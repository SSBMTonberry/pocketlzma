//
// Created by robin on 28.12.2020.
//

#ifndef POCKETLZMA_LZOPTIONS_HPP
#define POCKETLZMA_LZOPTIONS_HPP

namespace plz
{
    class LzOptions
    {
        public:
            /// Extra amount of data to keep available before the "actual"
            /// dictionary.
            size_t beforeSize;

            /// Size of the history buffer
            size_t dictSize;

            /// Extra amount of data to keep available after the "actual"
            /// dictionary.
            size_t afterSize;

            /// Maximum length of a match that the LZ-based encoder can accept.
            /// This is used to extend matches of length nice_len to the
            /// maximum possible length.
            size_t matchLenMax;

            /// Match finder will search matches up to this length.
            /// This must be less than or equal to match_len_max.
            size_t niceLen;

            /// Type of the match finder to use
            MatchFinder matchFinder;

            /// Maximum search depth
            uint32_t depth;

            /// TODO: Comment
            const uint8_t *presetDict;

            uint32_t presetDictSize;

            /*! set_lz_options */
            inline void setOptions(const LzmaOptions &options);
    };

    void LzOptions::setOptions(const LzmaOptions &options)
    {
        // LZ encoder initialization does the validation for these so we
        // don't need to validate here.
        beforeSize = OPTS;
        dictSize = options.dictSize;
        afterSize = LOOP_INPUT_MAX;
        matchLenMax = MATCH_LEN_MAX;
        niceLen = options.niceLen;
        matchFinder = options.mf;
        depth = options.depth;
        presetDict = options.presetDict;
        presetDictSize = options.presetDictSize;
    }
}

#endif //POCKETLZMA_LZOPTIONS_HPP
