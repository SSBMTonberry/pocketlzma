//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LENGTHENCODER_HPP
#define POCKETLZMA_LENGTHENCODER_HPP

namespace plz
{
    class LengthEncoder
    {
        public:
            LengthEncoder() = default;

            probability choice;
            probability choice2;
            probability low[POS_STATES_MAX][LEN_LOW_SYMBOLS];
            probability mid[POS_STATES_MAX][LEN_MID_SYMBOLS];
            probability high[LEN_HIGH_SYMBOLS];

            uint32_t prices[POS_STATES_MAX][LEN_SYMBOLS];
            uint32_t tableSize;
            uint32_t counters[POS_STATES_MAX];

            /*! length_encoder_reset */
            inline void reset(const uint32_t num_pos_states, const bool fast_mode);
            /*! get_len_price */
            inline uint32_t getLenPrice(const uint32_t len, const uint32_t pos_state);
            /*! length_update_prices */
            inline void updatePrices(const uint32_t pos_state);
    };

    void LengthEncoder::reset(const uint32_t num_pos_states, const bool fast_mode)
    {
        BitReset(choice);
        BitReset(choice2);

        for (size_t pos_state = 0; pos_state < num_pos_states; ++pos_state) {
            BittreeReset(low[pos_state], LEN_LOW_BITS);
            BittreeReset(mid[pos_state], LEN_MID_BITS);
        }

        BittreeReset(high, LEN_HIGH_BITS);

        if (!fast_mode)
            for (uint32_t pos_state = 0; pos_state < num_pos_states;
                 ++pos_state)
                updatePrices(pos_state);

        return;
    }

    void LengthEncoder::updatePrices(const uint32_t pos_state)
    {
        const uint32_t table_size = table_size;
        counters[pos_state] = table_size;

        const uint32_t a0 = Price::RcBit0Price(choice);
        const uint32_t a1 = Price::RcBit1Price(choice);
        const uint32_t b0 = a1 + Price::RcBit0Price(choice2);
        const uint32_t b1 = a1 + Price::RcBit1Price(choice2);
        uint32_t *const p = prices[pos_state];

        uint32_t i;
        for (i = 0; i < table_size && i < LEN_LOW_SYMBOLS; ++i)
            p[i] = a0 + Price::RcBittreePrice(low[pos_state],
                                              LEN_LOW_BITS, i);

        for (; i < table_size && i < LEN_LOW_SYMBOLS + LEN_MID_SYMBOLS; ++i)
            p[i] = b0 + Price::RcBittreePrice(mid[pos_state],
                                              LEN_MID_BITS, i - LEN_LOW_SYMBOLS);

        for (; i < table_size; ++i)
            p[i] = b1 + Price::RcBittreePrice(high, LEN_HIGH_BITS,
                                              i - LEN_LOW_SYMBOLS - LEN_MID_SYMBOLS);

    }

    uint32_t LengthEncoder::getLenPrice(const uint32_t len, const uint32_t pos_state)
    {
        // NOTE: Unlike the other price tables, length prices are updated
        // in lzma_encoder.c
        return prices[pos_state][len - MATCH_LEN_MIN];
    }
}

#endif //POCKETLZMA_LENGTHENCODER_HPP
