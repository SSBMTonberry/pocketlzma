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

            /*! make_literal */
            inline void makeLiteral();

            inline void makeShortRep();

            /*! Based on macro: is_short_rep */
            inline bool isShortRep();
    };

    void LzmaOptimal::makeLiteral()
    {
        backPrev = UINT32_MAX;
        prev1IsLiteral = false;
    }

    void LzmaOptimal::makeShortRep()
    {
        backPrev = 0;
        prev1IsLiteral = false;
    }

    bool LzmaOptimal::isShortRep()
    {
        return backPrev == 0;
    }
}

#endif //POCKETLZMA_LZMAOPTIMAL_HPP
