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
            enum class Symbol : uint8_t
            {
                RcBit0,
                RcBit1,
                RcDirect0,
                RcDirect1,
                RcFlush
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
            inline void bittree(probability *probs, uint32_t bit_count, uint32_t symbol);
            /*! rc_bittree_reverse */
            inline void bittreeReverse(probability *probs, uint32_t bit_count, uint32_t symbol);
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

            #error IMPLEMENT ALL FUNCTIONS ABOVE!

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
}

#endif //POCKETLZMA_RANGEENCODER_HPP
