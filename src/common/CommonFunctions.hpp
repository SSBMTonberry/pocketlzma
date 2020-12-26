//
// Created by robin on 24.12.2020.
//

#ifndef POCKETLZMA_COMMONFUNCTIONS_HPP
#define POCKETLZMA_COMMONFUNCTIONS_HPP

namespace plz
{
    // These allow helping the compiler in some often-executed branches, whose
    // result is almost always the same.
    #ifdef __GNUC__
    #	define likely(expr) __builtin_expect(expr, true)
    #	define unlikely(expr) __builtin_expect(expr, false)
    #else
    #	define likely(expr) (expr)
    #	define unlikely(expr) (expr)
    #endif

    #define change_pair(small_dist, big_dist) \
	(((big_dist) >> 7) > (small_dist))

    /********************
     * GNU C extensions *
     ********************/

    /*
     * GNU C extensions are used conditionally in the public API. It doesn't
     * break anything if these are sometimes enabled and sometimes not, only
     * affects warnings and optimizations.
     */
    #if defined(__GNUC__) && __GNUC__ >= 3
    #	ifndef lzma_attribute
    #		define lzma_attribute(attr) __attribute__(attr)
    #	endif

        /* warn_unused_result was added in GCC 3.4. */
    #	ifndef lzma_attr_warn_unused_result
    #		if __GNUC__ == 3 && __GNUC_MINOR__ < 4
    #			define lzma_attr_warn_unused_result
    #		endif
    #	endif

    #else
        #	ifndef lzma_attribute
    #		define lzma_attribute(attr)
    #	endif
    #endif

    #ifndef lzma_attr_pure
    #	define lzma_attr_pure lzma_attribute((__pure__))
    #endif

    #ifndef lzma_attr_const
    #	define lzma_attr_const lzma_attribute((__const__))
    #endif

    #ifndef lzma_attr_warn_unused_result
    #	define lzma_attr_warn_unused_result \
            lzma_attribute((__warn_unused_result__))
    #endif

    /*! NOTE: Avoid using MIN() and MAX(), because even conditionally defining
     *  those macros can cause some portability trouble, since on some systems
     *  the system headers insist defining their own versions.
     */
    #define my_min(x, y) ((x) < (y) ? (x) : (y))
    #define my_max(x, y) ((x) > (y) ? (x) : (y))

    /*! Macro to compare if the first two bytes in two buffers differ. This is
     *  needed in lzma_lzma_optimum_*() to test if the match is at least
     *  MATCH_LEN_MIN bytes. Unaligned access gives tiny gain so there's no
     *  reason to not use it when it is supported.
     */
    #define not_equal_16(a, b) \
		((a)[0] != (b)[0] || (a)[1] != (b)[1])

    /// Locate the literal coder for the next literal byte. The choice depends on
    ///   - the lowest literal_pos_bits bits of the position of the current
    ///     byte; and
    ///   - the highest literal_context_bits bits of the previous byte.
    #define literal_subcoder(probs, lc, lp_mask, pos, prev_byte) \
        ((probs)[(((pos) & (lp_mask)) << (lc)) \
                + ((uint32_t)(prev_byte) >> (8U - (lc)))])

    /// Indicate that the latest state was a literal.
    #define update_literal(state) \
        state = ((state) <= STATE_SHORTREP_LIT_LIT \
                ? STATE_LIT_LIT \
                : ((state) <= STATE_LIT_SHORTREP \
                    ? (state) - 3 \
                    : (state) - 6))

    /// Indicate that the latest state was a match.
    //#define update_match(state) \
    //    state = ((state) < LIT_STATES ? STATE_LIT_MATCH : STATE_NONLIT_MATCH)

    /// Indicate that the latest state was a long repeated match.
    //#define update_long_rep(state) \
    //    state = ((state) < LIT_STATES ? STATE_LIT_LONGREP : STATE_NONLIT_REP)

    /// Indicate that the latest state was a short match.
    //#define update_short_rep(state) \
    //    state = ((state) < LIT_STATES ? STATE_LIT_SHORTREP : STATE_NONLIT_REP)

    /// Test if the previous state was a literal.
    //#define is_literal_state(state) \
    //    ((state) < LIT_STATES)



    /*! Indicate that the latest state was a match. */
    static inline void UpdateMatch(LzmaState &state)
    {
        state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitMatch: LzmaState::NonLitMatch;
    }

    /*! Indicate that the latest state was a long repeated match. */
    static inline void UpdateLongRep(LzmaState &state)
    {
        state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitLongRep: LzmaState::NonLitRep;
    }

    /*! Indicate that the latest state was a short match. */
    static inline void UpdateShortRep(LzmaState &state)
    {
        state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitShortRep: LzmaState::NonLitRep;
    }

    /*! Test if the previous state was a literal. */
    static inline bool IsLiteralState(LzmaState state)
    {
        return (uint8_t)state < LIT_STATES;
    }

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

    /*! Find out how many equal bytes the two buffers have.
     *
     * \param      buf1    First buffer
     * \param      buf2    Second buffer
     * \param      len     How many bytes have already been compared and will
     *                     be assumed to match
     * \param      limit   How many bytes to compare at most, including the
     *                     already-compared bytes. This must be significantly
     *                     smaller than UINT32_MAX to avoid integer overflows.
     *                     Up to LZMA_MEMCMPLEN_EXTRA bytes may be read past
     *                     the specified limit from both buf1 and buf2.
     *
     * \return     Number of equal bytes in the buffers is returned.
     *             This is always at least len and at most limit.
     *
     * \note       LZMA_MEMCMPLEN_EXTRA defines how many extra bytes may be read.
     *             It's rounded up to 2^n. This extra amount needs to be
     *             allocated in the buffers being used. It needs to be
     *             initialized too to keep Valgrind quiet.
     */
    static inline uint32_t lzma_attribute((__always_inline__))
            LzmaMemcmplen(const uint8_t *buf1, const uint8_t *buf2, uint32_t len, uint32_t limit)
    {
        assert(len <= limit);
        assert(limit <= UINT32_MAX / 2);

        // Simple portable version that doesn't use unaligned access.
        # define LZMA_MEMCMPLEN_EXTRA 0
        while (len < limit && buf1[len] == buf2[len])
            ++len;

        return len;

    }

    extern const uint8_t lzma_fastpos[1 << FASTPOS_BITS];


#define fastpos_shift(extra, n) \
	((extra) + (n) * (FASTPOS_BITS - 1))

#define fastpos_limit(extra, n) \
	(UINT32_C(1) << (FASTPOS_BITS + fastpos_shift(extra, n)))

#define fastpos_result(dist, extra, n) \
	(uint32_t)(lzma_fastpos[(dist) >> fastpos_shift(extra, n)]) \
			+ 2 * fastpos_shift(extra, n)


    static inline uint32_t GetDistSlot(uint32_t dist)
    {
        // If it is small enough, we can pick the result directly from
        // the precalculated table.
        if (dist < fastpos_limit(0, 0))
            return lzma_fastpos[dist];

        if (dist < fastpos_limit(0, 1))
            return fastpos_result(dist, 0, 1);

        return fastpos_result(dist, 0, 2);
    }

    /*! Uses FULL_DISTANCES_BITS */
    static inline uint32_t GetDistSlot2(uint32_t dist)
    {
        assert(dist >= FULL_DISTANCES);

        if (dist < fastpos_limit(FULL_DISTANCES_BITS - 1, 0))
            return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 0);

        if (dist < fastpos_limit(FULL_DISTANCES_BITS - 1, 1))
            return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 1);

        return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 2);
    }
}

#endif //POCKETLZMA_COMMONFUNCTIONS_HPP
