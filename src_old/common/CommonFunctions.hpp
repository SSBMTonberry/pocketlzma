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

    /// \brief      Return if expression doesn't evaluate to LZMA_OK
    ///
    /// There are several situations where we want to return immediately
    /// with the value of expr if it isn't LZMA_OK. This macro shortens
    /// the code a little.
    #define return_if_error(expr) \
    do { \
        const StatusCode ret_ = (expr); \
        if (ret_ != StatusCode::Ok) \
            return ret_; \
    } while (0)

    /// Indicate that the latest state was a literal.
    //#define update_literal(state) \
    //    state = ((state) <= STATE_SHORTREP_LIT_LIT \
    //            ? STATE_LIT_LIT \
    //            : ((state) <= STATE_LIT_SHORTREP \
    //                ? (state) - 3 \
    //                : (state) - 6))



    /*! Based on the update_literal macro.
     * Indicate that the latest state was a literal.
     */
    static inline void UpdateLiteral(LzmaState &state)
    {
        uint8_t s = (uint8_t) state;
        uint8_t STATE_SHORTREP_LIT_LIT = (uint8_t) LzmaState::ShortRepLitLit;
        uint8_t STATE_LIT_LIT = (uint8_t) LzmaState::LitLit;
        uint8_t STATE_LIT_SHORTREP = (uint8_t) LzmaState::LitShortRep;
        uint8_t newS = (s <= STATE_SHORTREP_LIT_LIT) ? STATE_LIT_LIT : ((s <= STATE_LIT_SHORTREP) ? s - 3 : s - 6);
        state = (LzmaState) newS;
    }

    /*! Based on get_dist_state - macro to get the index of the appropriate probability array.*/
    static inline uint32_t GetDistState(uint32_t len)
    {
        return (len < (DIST_STATES + MATCH_LEN_MIN)) ? (len - MATCH_LEN_MIN) : (DIST_STATES - 1);
    }

    /*! Indicate that the latest state was a match. */
    static inline void UpdateMatch(LzmaState &state)
    {
        state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitMatch : LzmaState::NonLitMatch;
    }

    /*! Indicate that the latest state was a long repeated match. */
    static inline void UpdateLongRep(LzmaState &state)
    {
        state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitLongRep : LzmaState::NonLitRep;
    }

    /*! Indicate that the latest state was a short match. */
    static inline void UpdateShortRep(LzmaState &state)
    {
        state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitShortRep : LzmaState::NonLitRep;
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

    static inline void LiteralInit(probability (*probs)[LITERAL_CODER_SIZE],
                                   uint32_t lc, uint32_t lp)
    {
        assert(lc + lp <= LZMA_LCLP_MAX);

        const uint32_t coders = 1U << (lc + lp);

        for (uint32_t i = 0; i < coders; ++i)
            for (uint32_t j = 0; j < LITERAL_CODER_SIZE; ++j)
                BitReset(probs[i][j]);

        return;
    }

    static inline size_t LzmaBufcpy(const uint8_t * in, size_t * in_pos, size_t in_size,
                                    uint8_t * out, size_t * out_pos, size_t out_size)
    {
        const size_t in_avail = in_size - *in_pos;
        const size_t out_avail = out_size - *out_pos;
        const size_t copy_size = my_min(in_avail, out_avail);

        // Call memcpy() only if there is something to copy. If there is
        // nothing to copy, in or out might be NULL and then the memcpy()
        // call would trigger undefined behavior.
        if (copy_size > 0)
            memcpy(out + *out_pos, in + *in_pos, copy_size);

        *in_pos += copy_size;
        *out_pos += copy_size;

        return copy_size;
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
}

#include "Fastpos.hpp"

#endif //POCKETLZMA_COMMONFUNCTIONS_HPP
