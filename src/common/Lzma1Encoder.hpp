//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMA1ENCODER_HPP
#define POCKETLZMA_LZMA1ENCODER_HPP

namespace plz
{
    class Lzma1Encoder
    {
        public:
            Lzma1Encoder() = default;

            RangeEncoder rc;

            /// State
            LzmaState state;

            /// The four most recent match distances
            uint32_t reps[REPS];

            /// Array of match candidates
            LzmaMatch matches[MATCH_LEN_MAX + 1];

            /// Number of match candidates in matches[]
            uint32_t matchesCount;

            /// Variable to hold the length of the longest match between calls
            /// to lzma_lzma_optimum_*().
            uint32_t longestMatchLength;

            /// True if using getoptimumfast
            bool fastMode;

            /// True if the encoder has been initialized by encoding the first
            /// byte as a literal.
            bool isInitialized;

            /// True if the range encoder has been flushed, but not all bytes
            /// have been written to the output buffer yet.
            bool isFlushed;

            uint32_t posMask;         ///< (1 << pos_bits) - 1
            uint32_t literalContextBits;
            uint32_t literalPosMask;

            // These are the same as in lzma_decoder.c. See comments there.
            probability literal[LITERAL_CODERS_MAX][LITERAL_CODER_SIZE];
            probability isMatch[STATES][POS_STATES_MAX];
            probability isRep[STATES];
            probability isRep0[STATES];
            probability isRep1[STATES];
            probability isRep2[STATES];
            probability isRep0Long[STATES][POS_STATES_MAX];
            probability distSlot[DIST_STATES][DIST_SLOTS];
            probability distSpecial[FULL_DISTANCES - DIST_MODEL_END];
            probability distAlign[ALIGN_SIZE];

            // These are the same as in lzma_decoder.c except that the encoders
            // include also price tables.
            LengthEncoder matchLenEncoder;
            LengthEncoder repLenEncoder;

            // Price tables
            uint32_t dist_slot_prices[DIST_STATES][DIST_SLOTS];
            uint32_t dist_prices[DIST_STATES][FULL_DISTANCES];
            uint32_t dist_table_size;
            uint32_t match_price_count;

            uint32_t align_prices[ALIGN_SIZE];
            uint32_t align_price_count;

            // Optimal
            uint32_t opts_end_index;
            uint32_t opts_current_index;
            LzmaOptimal opts[OPTS];
    };
}

#endif //POCKETLZMA_LZMA1ENCODER_HPP
