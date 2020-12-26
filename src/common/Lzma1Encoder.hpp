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
            uint32_t distSlotPrices[DIST_STATES][DIST_SLOTS];
            uint32_t distPrices[DIST_STATES][FULL_DISTANCES];
            uint32_t distTableSize;
            uint32_t matchPriceCount;

            uint32_t alignPrices[ALIGN_SIZE];
            uint32_t alignPriceCount;

            // Optimal
            uint32_t optsEndIndex;
            uint32_t optsCurrentIndex;
            LzmaOptimal opts[OPTS];

            /*! encode_init */
            inline bool init(LzmaMF *mf);

            /*! lzma_lzma_optimum_normal */
            inline void lzmaOptimumNormal(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position);

            /*! lzma_lzma_optimum_fast */
            inline void lzmaOptimumFast(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res);

        private:
            /*! fill_dist_prices */
            inline void fillDistPrices();

            /*! fill_align_prices */
            inline void fillAlignPrices();

            /*! helper1
             *  A horrible function doing something important.
             * */
            inline uint32_t helper1(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position);

            /*! helper2
             *  Another horrible function doing something important.*/
            inline uint32_t helper2(uint32_t *reps, const uint8_t *buf,
                    uint32_t len_end, uint32_t position, const uint32_t cur,
                    const uint32_t nice_len, const uint32_t buf_avail_full);

            /*! get_literal_price */
            inline uint32_t getLiteralPrice(const uint32_t pos, const uint32_t prev_byte, const bool match_mode,
                              uint32_t match_byte, uint32_t symbol);

    };

    bool Lzma1Encoder::init(LzmaMF *mf)
    {
        assert(mf->position() == 0);

        if (mf->readPos == mf->readLimit)
        {
            if (mf->action == LzmaAction::Run)
                return false; // We cannot do anything.

            // We are finishing (we cannot get here when flushing).
            assert(mf->writePos == mf->readPos);
            assert(mf->action == LzmaAction::Finish);
        }
        else
        {
            // Do the actual initialization. The first LZMA symbol must
            // always be a literal.
            mf->mfSkip(1);
            mf->readAhead = 0;
            rc.bit(&isMatch[0][0], 0);
            rc.bittree(literal[0], 8, mf->buffer[0]);
        }

        // Initialization is done (except if empty file).
        isInitialized = true;

        return true;
    }

    void Lzma1Encoder::lzmaOptimumFast(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res)
    {
        const uint32_t nice_len = mf->niceLen;

        uint32_t len_main;
        uint32_t matches_count;
        if (mf->readAhead == 0) {
            len_main = mf->mfFind(&matches_count, matches);
        } else {
            assert(mf->readAhead == 1);
            len_main = longestMatchLength;
            matches_count = matches_count;
        }

        const uint8_t *buf = mf->ptr() - 1;
        const uint32_t buf_avail = my_min(mf->avail() + 1, MATCH_LEN_MAX);

        if (buf_avail < 2) {
            // There's not enough input left to encode a match.
            *back_res = UINT32_MAX;
            *len_res = 1;
            return;
        }

        // Look for repeated matches; scan the previous four match distances
        uint32_t rep_len = 0;
        uint32_t rep_index = 0;

        for (uint32_t i = 0; i < REPS; ++i)
        {
            // Pointer to the beginning of the match candidate
            const uint8_t *const buf_back = buf - reps[i] - 1;

            // If the first two bytes (2 == MATCH_LEN_MIN) do not match,
            // this rep is not useful.
            if (not_equal_16(buf, buf_back))
                continue;

            // The first two bytes matched.
            // Calculate the length of the match.
            const uint32_t len = LzmaMemcmplen(
                    buf, buf_back, 2, buf_avail);

            // If we have found a repeated match that is at least
            // nice_len long, return it immediately.
            if (len >= nice_len) {
                *back_res = i;
                *len_res = len;
                mf->mfSkip(len - 1);
                return;
            }

            if (len > rep_len) {
                rep_index = i;
                rep_len = len;
            }
        }

        // We didn't find a long enough repeated match. Encode it as a normal
        // match if the match length is at least nice_len.
        if (len_main >= nice_len) {
            *back_res = matches[matches_count - 1].dist + REPS;
            *len_res = len_main;
            mf->mfSkip(len_main - 1);
            return;
        }

        uint32_t back_main = 0;
        if (len_main >= 2) {
            back_main = matches[matches_count - 1].dist;

            while (matches_count > 1 && len_main ==
                                        matches[matches_count - 2].len + 1) {
                if (!change_pair(matches[
                                         matches_count - 2].dist,
                                 back_main))
                    break;

                --matches_count;
                len_main = matches[matches_count - 1].len;
                back_main = matches[matches_count - 1].dist;
            }

            if (len_main == 2 && back_main >= 0x80)
                len_main = 1;
        }

        if (rep_len >= 2)
        {
            if (rep_len + 1 >= len_main ||
               (rep_len + 2 >= len_main && back_main > (1 << 9)) ||
               (rep_len + 3 >= len_main && back_main > (1 << 15)))
            {
                *back_res = rep_index;
                *len_res = rep_len;
                mf->mfSkip(rep_len - 1);
                return;
            }
        }

        if (len_main < 2 || buf_avail <= 2) {
            *back_res = UINT32_MAX;
            *len_res = 1;
            return;
        }

        // Get the matches for the next byte. If we find a better match,
        // the current byte is encoded as a literal.
        longestMatchLength = mf->mfFind(&matches_count, matches);

        if (longestMatchLength >= 2) {
            const uint32_t new_dist = matches[
                    matches_count - 1].dist;

            if ((longestMatchLength >= len_main
                 && new_dist < back_main)
                || (longestMatchLength == len_main + 1
                    && !change_pair(back_main, new_dist))
                || (longestMatchLength > len_main + 1)
                || (longestMatchLength + 1 >= len_main
                    && len_main >= 3
                    && change_pair(new_dist, back_main))) {
                *back_res = UINT32_MAX;
                *len_res = 1;
                return;
            }
        }

        // In contrast to LZMA SDK, dictionary could not have been moved
        // between mf_find() calls, thus it is safe to just increment
        // the old buf pointer instead of recalculating it with mf_ptr().
        ++buf;

        const uint32_t limit = my_max(2, len_main - 1);

        for (uint32_t i = 0; i < REPS; ++i) {
            if (memcmp(buf, buf - reps[i] - 1, limit) == 0) {
                *back_res = UINT32_MAX;
                *len_res = 1;
                return;
            }
        }

        *back_res = back_main + REPS;
        *len_res = len_main;
        mf->mfSkip(len_main - 2);
    }

    void Lzma1Encoder::lzmaOptimumNormal(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position)
    {
        // If we have symbols pending, return the next pending symbol.
        if (optsEndIndex != optsCurrentIndex)
        {
            assert(mf->readAhead > 0);
            *len_res = opts[optsCurrentIndex].posPrev
                       - optsCurrentIndex;
            *back_res = opts[optsCurrentIndex].backPrev;
            optsCurrentIndex = opts[
                    optsCurrentIndex].posPrev;
            return;
        }

        // Update the price tables. In LZMA SDK <= 4.60 (and possibly later)
        // this was done in both initialization function and in the main loop.
        // In liblzma they were moved into this single place.
        if (mf->readAhead == 0)
        {
            if (matchPriceCount >= (1 << 7))
                fillDistPrices();

            if (alignPriceCount >= ALIGN_SIZE)
                fillAlignPrices();
        }

        // TODO: This needs quite a bit of cleaning still. But splitting
        // the original function into two pieces makes it at least a little
        // more readable, since those two parts don't share many variables.

        uint32_t len_end = helper1(mf, back_res, len_res, position);
        if (len_end == UINT32_MAX)
            return;

        uint32_t reps[REPS];
        memcpy(reps, reps, sizeof(reps));

        uint32_t cur;
        for (cur = 1; cur < len_end; ++cur) {
            assert(cur < OPTS);

            longestMatchLength = mf->mfFind(&matchesCount, matches);

            if (longestMatchLength >= mf->niceLen)
                break;

            len_end = helper2(coder, reps, mf_ptr(mf) - 1, len_end,
                              position + cur, cur, mf->nice_len,
                              my_min(mf_avail(mf) + 1, OPTS - 1 - cur));
        }

        backward(coder, len_res, back_res, cur);
    }

    void Lzma1Encoder::fillDistPrices()
    {
        for (uint32_t dist_state = 0; dist_state < DIST_STATES; ++dist_state)
        {

            uint32_t *const dist_slot_prices
                    = distSlotPrices[dist_state];

            // Price to encode the dist_slot.
            for (uint32_t dist_slot = 0; dist_slot < distTableSize; ++dist_slot)
            {
                dist_slot_prices[dist_slot] = Price::RcBittreePrice(distSlot[dist_state], DIST_SLOT_BITS, dist_slot);
            }

            // For matches with distance >= FULL_DISTANCES, add the price
            // of the direct bits part of the match distance. (Align bits
            // are handled by fill_align_prices()).
            for (uint32_t dist_slot = DIST_MODEL_END; dist_slot < distTableSize; ++dist_slot)
                dist_slot_prices[dist_slot] += Price::RcDirectPrice(((dist_slot >> 1) - 1) - ALIGN_BITS);

            // Distances in the range [0, 3] are fully encoded with
            // dist_slot, so they are used for dist_prices
            // as is.
            for (uint32_t i = 0; i < DIST_MODEL_START; ++i)
                distPrices[dist_state][i] = dist_slot_prices[i];
        }

        // Distances in the range [4, 127] depend on dist_slot and
        // dist_special. We do this in a loop separate from the above
        // loop to avoid redundant calls to get_dist_slot().
        for (uint32_t i = DIST_MODEL_START; i < FULL_DISTANCES; ++i)
        {
            const uint32_t dist_slot = GetDistSlot(i);
            const uint32_t footer_bits = ((dist_slot >> 1) - 1);
            const uint32_t base = (2 | (dist_slot & 1)) << footer_bits;
            const uint32_t price = Price::RcBittreeReversePrice(distSpecial + base - dist_slot - 1, footer_bits, i - base);

            for (uint32_t dist_state = 0; dist_state < DIST_STATES;
                 ++dist_state)
            {
                distPrices[dist_state][i] = price + distSlotPrices[dist_state][dist_slot];
            }
        }

        matchPriceCount = 0;
    }

    void Lzma1Encoder::fillAlignPrices()
    {
        for (uint32_t i = 0; i < ALIGN_SIZE; ++i)
        {
            alignPrices[i] = Price::RcBittreeReversePrice(distAlign, ALIGN_BITS, i);
        }

        alignPriceCount = 0;
    }

    uint32_t Lzma1Encoder::helper1(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position)
    {
        const uint32_t nice_len = mf->niceLen;

        uint32_t len_main;
        uint32_t matches_count;

        if (mf->readAhead == 0) {
            len_main = mf->mfFind(&matches_count, matches);
        } else {
            assert(mf->readAhead == 1);
            len_main = longestMatchLength;
            matches_count = matches_count;
        }

        const uint32_t buf_avail = my_min(mf->avail() + 1, MATCH_LEN_MAX);
        if (buf_avail < 2) {
            *back_res = UINT32_MAX;
            *len_res = 1;
            return UINT32_MAX;
        }

        const uint8_t *const buf = mf->ptr() - 1;

        uint32_t rep_lens[REPS];
        uint32_t rep_max_index = 0;

        for (uint32_t i = 0; i < REPS; ++i) {
            const uint8_t *const buf_back = buf - reps[i] - 1;

            if (not_equal_16(buf, buf_back)) {
                rep_lens[i] = 0;
                continue;
            }

            rep_lens[i] = LzmaMemcmplen(buf, buf_back, 2, buf_avail);

            if (rep_lens[i] > rep_lens[rep_max_index])
                rep_max_index = i;
        }

        if (rep_lens[rep_max_index] >= nice_len) {
            *back_res = rep_max_index;
            *len_res = rep_lens[rep_max_index];
            mf->mfSkip(*len_res - 1);
            return UINT32_MAX;
        }


        if (len_main >= nice_len) {
            *back_res = matches[matches_count - 1].dist + REPS;
            *len_res = len_main;
            mf->mfSkip(len_main - 1);
            return UINT32_MAX;
        }

        const uint8_t current_byte = *buf;
        const uint8_t match_byte = *(buf - reps[0] - 1);

        if (len_main < 2 && current_byte != match_byte
            && rep_lens[rep_max_index] < 2) {
            *back_res = UINT32_MAX;
            *len_res = 1;
            return UINT32_MAX;
        }

        opts[0].state = state;

        const uint32_t pos_state = position & posMask;

        opts[1].price = Price::RcBit0Price(isMatch[state][pos_state])
                               + getLiteralPrice(position, buf[-1], !is_literal_state(state), match_byte, current_byte);

        make_literal(&opts[1]);

        const uint32_t match_price = rc_bit_1_price(
                is_match[state][pos_state]);
        const uint32_t rep_match_price = match_price
                                         + rc_bit_1_price(is_rep[state]);

        if (match_byte == current_byte) {
            const uint32_t short_rep_price = rep_match_price
                                             + get_short_rep_price(
                    coder, state, pos_state);

            if (short_rep_price < opts[1].price) {
                opts[1].price = short_rep_price;
                make_short_rep(&opts[1]);
            }
        }

        const uint32_t len_end = my_max(len_main, rep_lens[rep_max_index]);

        if (len_end < 2) {
            *back_res = opts[1].backPrev;
            *len_res = 1;
            return UINT32_MAX;
        }

        opts[1].posPrev = 0;

        for (uint32_t i = 0; i < REPS; ++i)
            opts[0].backs[i] = reps[i];

        uint32_t len = len_end;
        do {
            opts[len].price = RC_INFINITY_PRICE;
        } while (--len >= 2);


        for (uint32_t i = 0; i < REPS; ++i) {
            uint32_t rep_len = rep_lens[i];
            if (rep_len < 2)
                continue;

            const uint32_t price = rep_match_price + get_pure_rep_price(
                    coder, i, state, pos_state);

            do {
                const uint32_t cur_and_len_price = price
                                                   + get_len_price(
                        &rep_len_encoder,
                        rep_len, pos_state);

                if (cur_and_len_price < opts[rep_len].price) {
                    opts[rep_len].price = cur_and_len_price;
                    opts[rep_len].pos_prev = 0;
                    opts[rep_len].back_prev = i;
                    opts[rep_len].prev_1_is_literal = false;
                }
            } while (--rep_len >= 2);
        }


        const uint32_t normal_match_price = match_price
                                            + rc_bit_0_price(is_rep[state]);

        len = rep_lens[0] >= 2 ? rep_lens[0] + 1 : 2;
        if (len <= len_main) {
            uint32_t i = 0;
            while (len > matches[i].len)
                ++i;

            for(; ; ++len) {
                const uint32_t dist = matches[i].dist;
                const uint32_t cur_and_len_price = normal_match_price
                                                   + get_dist_len_price(coder,
                                                                        dist, len, pos_state);

                if (cur_and_len_price < opts[len].price) {
                    opts[len].price = cur_and_len_price;
                    opts[len].pos_prev = 0;
                    opts[len].back_prev = dist + REPS;
                    opts[len].prev_1_is_literal = false;
                }

                if (len == matches[i].len)
                    if (++i == matches_count)
                        break;
            }
        }

        return len_end;
    }

    uint32_t Lzma1Encoder::getLiteralPrice(const uint32_t pos, const uint32_t prev_byte, const bool match_mode, uint32_t match_byte, uint32_t symbol)
    {
        const probability *const subcoder = literal_subcoder(literal, literalContextBits, literalPosMask, pos, prev_byte);

        uint32_t price = 0;

        if (!match_mode) {
            price = Price::RcBittreePrice(subcoder, 8, symbol);
        } else {
            uint32_t offset = 0x100;
            symbol += 1 << 8;

            do {
                match_byte <<= 1;

                const uint32_t match_bit = match_byte & offset;
                const uint32_t subcoder_index
                        = offset + match_bit + (symbol >> 8);
                const uint32_t bit = (symbol >> 7) & 1;
                price += Price::RcBitPrice(subcoder[subcoder_index], bit);

                symbol <<= 1;
                offset &= ~(match_byte ^ symbol);

            } while (symbol < (1 << 16));
        }

        return price;
    }

    uint32_t Lzma1Encoder::helper2(uint32_t *reps, const uint8_t *buf, uint32_t len_end, uint32_t position, const uint32_t cur, const uint32_t nice_len,
                          const uint32_t buf_avail_full)
    {
        uint32_t matches_count = matches_count;
        uint32_t new_len = longestMatchLength;
        uint32_t pos_prev = opts[cur].posPrev;
        LzmaState lzmaState;

        if (opts[cur].prev1IsLiteral) {
            --pos_prev;

            if (opts[cur].prev2) {
                lzmaState = opts[opts[cur].posPrev2].state;

                if (opts[cur].backPrev2 < REPS)
                    UpdateLongRep(lzmaState);
                else
                    UpdateMatch(lzmaState);

            } else {
                lzmaState = opts[pos_prev].state;
            }

            update_literal(lzmaState);

        } else {
            lzmaState = opts[pos_prev].state;
        }

        if (pos_prev == cur - 1) {
            if (is_short_rep(opts[cur]))
                UpdateShortRep(lzmaState);
            else
                update_literal(lzmaState);
        } else {
            uint32_t pos;
            if (opts[cur].prev1IsLiteral
                && opts[cur].prev2)
            {
                pos_prev = opts[cur].posPrev2;
                pos = opts[cur].backPrev2;
                UpdateLongRep(lzmaState);
            } else {
                pos = opts[cur].backPrev;
                if (pos < REPS)
                    UpdateLongRep(lzmaState);
                else
                    UpdateMatch(lzmaState);
            }

            if (pos < REPS)
            {
                reps[0] = opts[pos_prev].backs[pos];

                uint32_t i;
                for (i = 1; i <= pos; ++i)
                    reps[i] = opts[pos_prev].backs[i - 1];

                for (; i < REPS; ++i)
                    reps[i] = opts[pos_prev].backs[i];

            }
            else
            {
                reps[0] = pos - REPS;

                for (uint32_t i = 1; i < REPS; ++i)
                    reps[i] = opts[pos_prev].backs[i - 1];
            }
        }

        opts[cur].state = lzmaState;

        for (uint32_t i = 0; i < REPS; ++i)
            opts[cur].backs[i] = reps[i];

        const uint32_t cur_price = opts[cur].price;

        const uint8_t current_byte = *buf;
        const uint8_t match_byte = *(buf - reps[0] - 1);

        const uint32_t pos_state = position & posMask;

        const uint32_t cur_and_1_price = cur_price
                                         + Price::RcBit0Price(isMatch[(uint8_t)lzmaState][pos_state])
                                         + getLiteralPrice(position, buf[-1],
                                                             !is_literal_state(lzmaState), match_byte, current_byte);

        bool next_is_literal = false;

        if (cur_and_1_price < opts[cur + 1].price) {
            opts[cur + 1].price = cur_and_1_price;
            opts[cur + 1].pos_prev = cur;
            make_literal(&opts[cur + 1]);
            next_is_literal = true;
        }

        const uint32_t match_price = cur_price
                                     + rc_bit_1_price(is_match[lzmaState][pos_state]);
        const uint32_t rep_match_price = match_price
                                         + rc_bit_1_price(is_rep[lzmaState]);

        if (match_byte == current_byte
            && !(opts[cur + 1].pos_prev < cur
                 && opts[cur + 1].back_prev == 0)) {

            const uint32_t short_rep_price = rep_match_price
                                             + get_short_rep_price(coder, lzmaState, pos_state);

            if (short_rep_price <= opts[cur + 1].price) {
                opts[cur + 1].price = short_rep_price;
                opts[cur + 1].pos_prev = cur;
                make_short_rep(&opts[cur + 1]);
                next_is_literal = true;
            }
        }

        if (buf_avail_full < 2)
            return len_end;

        const uint32_t buf_avail = my_min(buf_avail_full, nice_len);

        if (!next_is_literal && match_byte != current_byte) { // speed optimization
            // try literal + rep0
            const uint8_t *const buf_back = buf - reps[0] - 1;
            const uint32_t limit = my_min(buf_avail_full, nice_len + 1);

            const uint32_t len_test = lzma_memcmplen(buf, buf_back, 1, limit) - 1;

            if (len_test >= 2) {
                lzma_lzma_state state_2 = lzmaState;
                update_literal(state_2);

                const uint32_t pos_state_next = (position + 1) & pos_mask;
                const uint32_t next_rep_match_price = cur_and_1_price
                                                      + rc_bit_1_price(is_match[state_2][pos_state_next])
                                                      + rc_bit_1_price(is_rep[state_2]);

                //for (; len_test >= 2; --len_test) {
                const uint32_t offset = cur + 1 + len_test;

                while (len_end < offset)
                    opts[++len_end].price = RC_INFINITY_PRICE;

                const uint32_t cur_and_len_price = next_rep_match_price
                                                   + get_rep_price(coder, 0, len_test,
                                                                   state_2, pos_state_next);

                if (cur_and_len_price < opts[offset].price) {
                    opts[offset].price = cur_and_len_price;
                    opts[offset].pos_prev = cur + 1;
                    opts[offset].back_prev = 0;
                    opts[offset].prev_1_is_literal = true;
                    opts[offset].prev_2 = false;
                }
                //}
            }
        }


        uint32_t start_len = 2; // speed optimization

        for (uint32_t rep_index = 0; rep_index < REPS; ++rep_index) {
            const uint8_t *const buf_back = buf - reps[rep_index] - 1;
            if (not_equal_16(buf, buf_back))
                continue;

            uint32_t len_test = lzma_memcmplen(buf, buf_back, 2, buf_avail);

            while (len_end < cur + len_test)
                opts[++len_end].price = RC_INFINITY_PRICE;

            const uint32_t len_test_temp = len_test;
            const uint32_t price = rep_match_price + get_pure_rep_price(
                    coder, rep_index, lzmaState, pos_state);

            do {
                const uint32_t cur_and_len_price = price
                                                   + get_len_price(&rep_len_encoder,
                                                                   len_test, pos_state);

                if (cur_and_len_price < opts[cur + len_test].price) {
                    opts[cur + len_test].price = cur_and_len_price;
                    opts[cur + len_test].pos_prev = cur;
                    opts[cur + len_test].back_prev = rep_index;
                    opts[cur + len_test].prev_1_is_literal = false;
                }
            } while (--len_test >= 2);

            len_test = len_test_temp;

            if (rep_index == 0)
                start_len = len_test + 1;


            uint32_t len_test_2 = len_test + 1;
            const uint32_t limit = my_min(buf_avail_full,
                                          len_test_2 + nice_len);
            // NOTE: len_test_2 may be greater than limit so the call to
            // lzma_memcmplen() must be done conditionally.
            if (len_test_2 < limit)
                len_test_2 = lzma_memcmplen(buf, buf_back, len_test_2, limit);

            len_test_2 -= len_test + 1;

            if (len_test_2 >= 2) {
                lzma_lzma_state state_2 = lzmaState;
                update_long_rep(state_2);

                uint32_t pos_state_next = (position + len_test) & pos_mask;

                const uint32_t cur_and_len_literal_price = price
                                                           + get_len_price(&rep_len_encoder,
                                                                           len_test, pos_state)
                                                           + rc_bit_0_price(is_match[state_2][pos_state_next])
                                                           + get_literal_price(coder, position + len_test,
                                                                               buf[len_test - 1], true,
                                                                               buf_back[len_test], buf[len_test]);

                update_literal(state_2);

                pos_state_next = (position + len_test + 1) & pos_mask;

                const uint32_t next_rep_match_price = cur_and_len_literal_price
                                                      + rc_bit_1_price(is_match[state_2][pos_state_next])
                                                      + rc_bit_1_price(is_rep[state_2]);

                //for(; len_test_2 >= 2; len_test_2--) {
                const uint32_t offset = cur + len_test + 1 + len_test_2;

                while (len_end < offset)
                    opts[++len_end].price = RC_INFINITY_PRICE;

                const uint32_t cur_and_len_price = next_rep_match_price
                                                   + get_rep_price(coder, 0, len_test_2,
                                                                   state_2, pos_state_next);

                if (cur_and_len_price < opts[offset].price) {
                    opts[offset].price = cur_and_len_price;
                    opts[offset].pos_prev = cur + len_test + 1;
                    opts[offset].back_prev = 0;
                    opts[offset].prev_1_is_literal = true;
                    opts[offset].prev_2 = true;
                    opts[offset].pos_prev_2 = cur;
                    opts[offset].back_prev_2 = rep_index;
                }
                //}
            }
        }


        //for (uint32_t len_test = 2; len_test <= new_len; ++len_test)
        if (new_len > buf_avail) {
            new_len = buf_avail;

            matches_count = 0;
            while (new_len > matches[matches_count].len)
                ++matches_count;

            matches[matches_count++].len = new_len;
        }


        if (new_len >= start_len) {
            const uint32_t normal_match_price = match_price
                                                + rc_bit_0_price(is_rep[lzmaState]);

            while (len_end < cur + new_len)
                opts[++len_end].price = RC_INFINITY_PRICE;

            uint32_t i = 0;
            while (start_len > matches[i].len)
                ++i;

            for (uint32_t len_test = start_len; ; ++len_test) {
                const uint32_t cur_back = matches[i].dist;
                uint32_t cur_and_len_price = normal_match_price
                                             + get_dist_len_price(coder,
                                                                  cur_back, len_test, pos_state);

                if (cur_and_len_price < opts[cur + len_test].price) {
                    opts[cur + len_test].price = cur_and_len_price;
                    opts[cur + len_test].pos_prev = cur;
                    opts[cur + len_test].back_prev
                            = cur_back + REPS;
                    opts[cur + len_test].prev_1_is_literal = false;
                }

                if (len_test == matches[i].len) {
                    // Try Match + Literal + Rep0
                    const uint8_t *const buf_back = buf - cur_back - 1;
                    uint32_t len_test_2 = len_test + 1;
                    const uint32_t limit = my_min(buf_avail_full,
                                                  len_test_2 + nice_len);

                    // NOTE: len_test_2 may be greater than limit
                    // so the call to lzma_memcmplen() must be
                    // done conditionally.
                    if (len_test_2 < limit)
                        len_test_2 = lzma_memcmplen(buf, buf_back,
                                                    len_test_2, limit);

                    len_test_2 -= len_test + 1;

                    if (len_test_2 >= 2) {
                        lzma_lzma_state state_2 = lzmaState;
                        update_match(state_2);
                        uint32_t pos_state_next
                                = (position + len_test) & pos_mask;

                        const uint32_t cur_and_len_literal_price = cur_and_len_price
                                                                   + rc_bit_0_price(
                                is_match[state_2][pos_state_next])
                                                                   + get_literal_price(coder,
                                                                                       position + len_test,
                                                                                       buf[len_test - 1],
                                                                                       true,
                                                                                       buf_back[len_test],
                                                                                       buf[len_test]);

                        update_literal(state_2);
                        pos_state_next = (pos_state_next + 1) & pos_mask;

                        const uint32_t next_rep_match_price
                                = cur_and_len_literal_price
                                  + rc_bit_1_price(
                                        is_match[state_2][pos_state_next])
                                  + rc_bit_1_price(is_rep[state_2]);

                        // for(; len_test_2 >= 2; --len_test_2) {
                        const uint32_t offset = cur + len_test + 1 + len_test_2;

                        while (len_end < offset)
                            opts[++len_end].price = RC_INFINITY_PRICE;

                        cur_and_len_price = next_rep_match_price
                                            + get_rep_price(coder, 0, len_test_2,
                                                            state_2, pos_state_next);

                        if (cur_and_len_price < opts[offset].price) {
                            opts[offset].price = cur_and_len_price;
                            opts[offset].pos_prev = cur + len_test + 1;
                            opts[offset].back_prev = 0;
                            opts[offset].prev_1_is_literal = true;
                            opts[offset].prev_2 = true;
                            opts[offset].pos_prev_2 = cur;
                            opts[offset].back_prev_2
                                    = cur_back + REPS;
                        }
                        //}
                    }

                    if (++i == matches_count)
                        break;
                }
            }
        }

        return len_end;
    }
}

#endif //POCKETLZMA_LZMA1ENCODER_HPP
