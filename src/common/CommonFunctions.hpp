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

    /*! Get the number of bytes that haven't been encoded yet (some of these
     *  bytes may have been ran through the match finder though).
     */
    static inline uint32_t MfUnencoded(const LzmaMF *mf)
    {
        return mf->writePos - mf->readPos + mf->readAhead;
    }

    // Macro to get the index of the appropriate probability array.
    // #define get_dist_state(len) \
    //     ((len) < DIST_STATES + MATCH_LEN_MIN \
    //         ? (len) - MATCH_LEN_MIN \
    //         : DIST_STATES - 1)
}

#endif //POCKETLZMA_COMMONFUNCTIONS_HPP
