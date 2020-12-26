//
// Created by robin on 26.12.2020.
// Probability price calculation
//
//

#ifndef POCKETLZMA_PRICE_HPP
#define POCKETLZMA_PRICE_HPP

namespace plz
{
    const uint8_t  RC_MOVE_REDUCING_BITS         {4};
    const uint8_t  RC_BIT_PRICE_SHIFT_BITS       {4};
    const uint32_t RC_PRICE_TABLE_SIZE           {RC_BIT_MODEL_TOTAL >> RC_MOVE_REDUCING_BITS};

    const uint32_t RC_INFINITY_PRICE             {1 << 30};

    /*! Collection of probability price calculations (static)
     *  Based on price.h
     */
    class Price
    {
        public:
            Price() = delete;

            /*! rc_bit_price */
            static inline uint32_t RcBitPrice(const probability prob, const uint32_t bit);
            /*! rc_bit_0_price */
            static inline uint32_t RcBit0Price(const probability prob);
            /*! rc_bit_1_price */
            static inline uint32_t RcBit1Price(const probability prob);
            /*! rc_bittree_price */
            static inline uint32_t RcBittreePrice(const probability *const probs,
                             const uint32_t bit_levels, uint32_t symbol);
            /*! rc_bittree_reverse_price */
            static inline uint32_t RcBittreeReversePrice(const probability *const probs,
                                     uint32_t bit_levels, uint32_t symbol);

            /*! rc_direct_price */
            static inline uint32_t RcDirectPrice(const uint32_t bits);

        private:
            static const uint8_t lzma_rc_prices[RC_PRICE_TABLE_SIZE];

    };

    uint32_t Price::RcBitPrice(const probability prob, const uint32_t bit)
    {
        return lzma_rc_prices[(prob ^ ((0 - bit)
                                       & (RC_BIT_MODEL_TOTAL - 1))) >> RC_MOVE_REDUCING_BITS];
    }

    uint32_t Price::RcBit0Price(const probability prob)
    {
        return lzma_rc_prices[prob >> RC_MOVE_REDUCING_BITS];
    }

    uint32_t Price::RcBit1Price(const probability prob)
    {
        return lzma_rc_prices[(prob ^ (RC_BIT_MODEL_TOTAL - 1))
                >> RC_MOVE_REDUCING_BITS];
    }

    uint32_t Price::RcBittreePrice(const probability *const probs, const uint32_t bit_levels, uint32_t symbol)
    {
        uint32_t price = 0;
        symbol += 1 << bit_levels;

        do {
            const uint32_t bit = symbol & 1;
            symbol >>= 1;
            price += RcBitPrice(probs[symbol], bit);
        } while (symbol != 1);

        return price;
    }

    uint32_t Price::RcBittreeReversePrice(const probability *const probs, uint32_t bit_levels, uint32_t symbol)
    {
        uint32_t price = 0;
        uint32_t model_index = 1;

        do {
            const uint32_t bit = symbol & 1;
            symbol >>= 1;
            price += RcBitPrice(probs[model_index], bit);
            model_index = (model_index << 1) + bit;
        } while (--bit_levels != 0);

        return price;
    }

    uint32_t Price::RcDirectPrice(const uint32_t bits)
    {
        return bits << RC_BIT_PRICE_SHIFT_BITS;
    }
}

#endif //POCKETLZMA_PRICE_HPP
