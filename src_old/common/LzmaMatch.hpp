//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMAMATCH_HPP
#define POCKETLZMA_LZMAMATCH_HPP
namespace plz
{
    /*!
     * A table of these is used by the LZ-based encoder to hold
     * the length-distance pairs found by the match finder.
     */
    class LzmaMatch
    {
        public:
            uint32_t len;
            uint32_t dist;
    };
}

#endif //POCKETLZMA_LZMAMATCH_HPP
