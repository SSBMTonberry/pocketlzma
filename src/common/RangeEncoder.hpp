//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_RANGEENCODER_HPP
#define POCKETLZMA_RANGEENCODER_HPP

namespace plz
{
    class RangeEncoder
    {
        public:
            /*! Maximum number of symbols that can be put pending into lzma_range_encoder
                structure between calls to lzma_rc_encode(). For LZMA, 52+5 is enough
                (match with big distance and length followed by range encoder flush). */
            static const uint8_t RC_SYMBOLS_MAX {58};
            enum class Symbol : uint32_t
            {
                Bit0,
                Bit1,
                Direct0,
                Direct1,
                Flush
            };

            RangeEncoder() = default;

            uint64_t low;
            uint64_t cacheSize;
            uint32_t range;
            uint8_t cache;

            /// Number of symbols in the tables
            size_t count;

            /// rc_encode()'s position in the tables
            size_t pos;

            /// Symbols to encode
            Symbol symbols[RC_SYMBOLS_MAX];

            /// Probabilities associated with RC_BIT_0 or RC_BIT_1
            probability *probs[RC_SYMBOLS_MAX];

            //Functions

            /*! rc_reset */
            inline void reset();
            /*! rc_bit */
            inline void bit(probability *prob, uint32_t bit);
            /*! rc_bittree */
            inline void bittree(probability *probabilities, uint32_t bit_count, uint32_t symbol);
            /*! rc_bittree_reverse */
            inline void bittreeReverse(probability *probabilities, uint32_t bit_count, uint32_t symbol);
            /*! rc_direct */
            inline void direct(uint32_t value, uint32_t bit_count);
            /*! rc_flush */
            inline void flush();
            /*! rc_shift_low */
            inline bool shiftLow(uint8_t *out, size_t *out_pos, size_t out_size);
            /*! rc_encode */
            inline bool encode(uint8_t *out, size_t *out_pos, size_t out_size);
            /*! rc_pending */
            inline uint64_t pending();

            /*! Symbol to int converter*/
            inline uint32_t sint(Symbol s);
            /*! Symbol to int converter*/
            inline Symbol intToSymbol(uint32_t i);

    };

    void RangeEncoder::reset()
    {
        low = 0;
        cacheSize = 1;
        range = UINT32_MAX;
        cache = 0;
        count = 0;
        pos = 0;
    }

    void RangeEncoder::bit(probability *prob, uint32_t bit)
    {
        symbols[count] = (Symbol) bit;
        probs[count] = prob;
        ++count;
    }

    void RangeEncoder::bittree(probability *probabilities, uint32_t bit_count, uint32_t symbol)
    {
        uint32_t model_index = 1;

        do {
            const uint32_t b = (symbol >> --bit_count) & 1;
            bit(&probabilities[model_index], b);
            model_index = (model_index << 1) + b;
        } while (bit_count != 0);
    }

    void RangeEncoder::bittreeReverse(probability *probabilities, uint32_t bit_count, uint32_t symbol)
    {
        uint32_t model_index = 1;

        do {
            const uint32_t b = symbol & 1;
            symbol >>= 1;
            bit(&probabilities[model_index], b);
            model_index = (model_index << 1) + b;
        } while (--bit_count != 0);
    }

    void RangeEncoder::direct(uint32_t value, uint32_t bit_count)
    {
        do {
            symbols[count++] = intToSymbol(sint(Symbol::Direct0) + ((value >> --bit_count) & 1));
        } while (bit_count != 0);
    }

    void RangeEncoder::flush()
    {
        for (size_t i = 0; i < 5; ++i)
            symbols[count++] = Symbol::Flush;
    }

    bool RangeEncoder::shiftLow(uint8_t *out, size_t *out_pos, size_t out_size)
    {
        if ((uint32_t)(low) < (uint32_t)(0xFF000000)
            || (uint32_t)(low >> 32) != 0) {
            do {
                if (*out_pos == out_size)
                    return true;

                out[*out_pos] = cache + (uint8_t)(low >> 32);
                ++*out_pos;
                cache = 0xFF;

            } while (--cacheSize != 0);

            cache = (low >> 24) & 0xFF;
        }

        ++cacheSize;
        low = (low & 0x00FFFFFF) << RC_SHIFT_BITS;

        return false;
    }

    bool RangeEncoder::encode(uint8_t *out, size_t *out_pos, size_t out_size)
    {
        assert(count <= RC_SYMBOLS_MAX);

        while (pos < count) {
            // Normalize
            if (range < RC_TOP_VALUE) {
                if (shiftLow(out, out_pos, out_size))
                    return true;

                range <<= RC_SHIFT_BITS;
            }

            // Encode a bit
            switch (symbols[pos]) {
                case Symbol::Bit0: {
                    probability prob = *probs[pos];
                    range = (range >> RC_BIT_MODEL_TOTAL_BITS)
                                * prob;
                    prob += (RC_BIT_MODEL_TOTAL - prob) >> RC_MOVE_BITS;
                    *probs[pos] = prob;
                    break;
                }

                case Symbol::Bit1: {
                    probability prob = *probs[pos];
                    const uint32_t bound = prob * (range
                            >> RC_BIT_MODEL_TOTAL_BITS);
                    low += bound;
                    range -= bound;
                    prob -= prob >> RC_MOVE_BITS;
                    *probs[pos] = prob;
                    break;
                }

                case Symbol::Direct0:
                    range >>= 1;
                    break;

                case Symbol::Direct1:
                    range >>= 1;
                    low += range;
                    break;

                case Symbol::Flush:
                    // Prevent further normalizations.
                    range = UINT32_MAX;

                    // Flush the last five bytes (see rc_flush()).
                    do {
                        if (shiftLow(out, out_pos, out_size))
                            return true;
                    } while (++pos < count);

                    // Reset the range encoder so we are ready to continue
                    // encoding if we weren't finishing the stream.
                    reset();
                    return false;

                default:
                    assert(0);
                    break;
            }

            ++pos;
        }

        count = 0;
        pos = 0;

        return false;
    }

    uint64_t RangeEncoder::pending()
    {
        return cacheSize + 5 - 1;
    }

    uint32_t RangeEncoder::sint(RangeEncoder::Symbol s)
    {
        return static_cast<uint32_t>(s);
    }

    RangeEncoder::Symbol RangeEncoder::intToSymbol(uint32_t i)
    {
        return static_cast<RangeEncoder::Symbol>(i);
    }
}

#endif //POCKETLZMA_RANGEENCODER_HPP
