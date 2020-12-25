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
    };
}

#endif //POCKETLZMA_LENGTHENCODER_HPP
