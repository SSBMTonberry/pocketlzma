//
// Created by robin on 24.12.2020.
//

#ifndef POCKETLZMA_COMMONFUNCTIONS_HPP
#define POCKETLZMA_COMMONFUNCTIONS_HPP

namespace plz
{

    /*! Resets the probability so that both 0 and 1 have probability of 50 % */
    static inline void BitReset(probability &prob)
    {
        prob = RC_BIT_MODEL_TOTAL >> 1;
    }

    /*! This does the same for a complete bit tree. (A tree represented as an array.) */
    static inline void BittreeReset(probability *probs, uint32_t bit_levels)
    {
        for (uint32_t bt_i = 0; bt_i < (1 << (bit_levels)); ++bt_i)
		    BitReset((probs)[bt_i]);
    }
}

#endif //POCKETLZMA_COMMONFUNCTIONS_HPP
