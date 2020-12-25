//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMAOPTIMAL_HPP
#define POCKETLZMA_LZMAOPTIMAL_HPP

namespace plz
{
    class LzmaOptimal
    {
        public:
            LzmaOptimal() = default;
            LzmaState state;

            bool prev1IsLiteral;
            bool prev2;

            uint32_t posPrev2;
            uint32_t backPrev2;

            uint32_t price;
            uint32_t posPrev;  // pos_next;
            uint32_t backPrev;

            uint32_t backs[REPS];
    };
}

#endif //POCKETLZMA_LZMAOPTIMAL_HPP
