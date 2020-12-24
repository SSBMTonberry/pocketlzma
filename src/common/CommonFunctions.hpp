//
// Created by robin on 24.12.2020.
//

#ifndef POCKETLZMA_COMMONFUNCTIONS_HPP
#define POCKETLZMA_COMMONFUNCTIONS_HPP

namespace plz
{
    static inline StatusCode IsLclppbValid(const LzmaOptions *options)
    {
        if(options->lc <= LZMA_LCLP_MAX && options->lp <= LZMA_LCLP_MAX
           && options->lc + options->lp <= LZMA_LCLP_MAX
           && options->pb <= LZMA_PB_MAX)
            return StatusCode::Ok;

        return StatusCode::ErrorInLclppbCheckOnLzmaOptions;
    }
}

#endif //POCKETLZMA_COMMONFUNCTIONS_HPP
