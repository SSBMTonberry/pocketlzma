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

            /*! lzma_lzma_encoder_reset */
            StatusCode reset(const LzmaOptions *options);

            /*! encode_symbol */
            inline void encodeSymbol(LzmaMF *mf, uint32_t back, uint32_t len, uint32_t position);

            /*! encode_eopm */
            inline void encodeEopm(uint32_t position);

            /*! lzma_lzma_optimum_normal */
            inline void lzmaOptimumNormal(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position);

            /*! lzma_lzma_optimum_fast */
            inline void lzmaOptimumFast(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res);

            /*! lzma_lzma_encoder_create */
            inline StatusCode create(const LzmaOptions *options, LzOptions *lz_options);

        private:
            /*! fill_dist_prices */
            inline void fillDistPrices();

            /*! fill_align_prices */
            inline void fillAlignPrices();

            /*! helper1
             *  A horrible function doing something important.
             * */
            inline uint32_t optimumNormalHelper1(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position);

            /*! helper2
             *  Another horrible function doing something important.*/
            inline uint32_t optimumNormalHelper2(uint32_t *reps, const uint8_t *buf,
                                                 uint32_t len_end, uint32_t position, const uint32_t cur,
                                                 const uint32_t nice_len, const uint32_t buf_avail_full);

            /*! get_literal_price */
            inline uint32_t getLiteralPrice(const uint32_t pos, const uint32_t prev_byte, const bool match_mode,
                              uint32_t match_byte, uint32_t symbol);

            /*! get_short_rep_price */
            inline uint32_t getShortRepPrice(const LzmaState state, const uint32_t pos_state);

            /*! get_pure_rep_price */
            inline uint32_t getPureRepPrice(const uint32_t rep_index, const LzmaState state, uint32_t pos_state);

            /*! get_rep_price */
            inline uint32_t getRepPrice(const uint32_t rep_index, const uint32_t len, const LzmaState state,
                          const uint32_t pos_state);

            /*! get_dist_len_price */
            inline uint32_t getDistLenPrice(const uint32_t dist, const uint32_t len, const uint32_t pos_state);

            inline void backward(uint32_t * len_res, uint32_t * back_res, uint32_t cur);


            /*! literal
             *  Named literalFunc to not get confused with the variable named "literal"
             */
            inline void literalFunc(LzmaMF *mf, uint32_t position);

            inline void match(const uint32_t pos_state, const uint32_t distance, const uint32_t len);
            /*! rep_match - Repeated match */
            inline void repMatch(const uint32_t pos_state, const uint32_t rep, const uint32_t len);

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

        uint32_t len_end = optimumNormalHelper1(mf, back_res, len_res, position);
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

            len_end = optimumNormalHelper2(reps, mf->ptr() - 1, len_end,
                                           position + cur, cur, mf->niceLen,
                                           my_min(mf->avail() + 1, OPTS - 1 - cur));
        }

        backward(len_res, back_res, cur);
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

    uint32_t Lzma1Encoder::optimumNormalHelper1(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position)
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

        if (rep_lens[rep_max_index] >= nice_len)
        {
            *back_res = rep_max_index;
            *len_res = rep_lens[rep_max_index];
            mf->mfSkip(*len_res - 1);
            return UINT32_MAX;
        }


        if (len_main >= nice_len)
        {
            *back_res = matches[matches_count - 1].dist + REPS;
            *len_res = len_main;
            mf->mfSkip(len_main - 1);
            return UINT32_MAX;
        }

        const uint8_t current_byte = *buf;
        const uint8_t match_byte = *(buf - reps[0] - 1);

        if (len_main < 2 && current_byte != match_byte
            && rep_lens[rep_max_index] < 2)
        {
            *back_res = UINT32_MAX;
            *len_res = 1;
            return UINT32_MAX;
        }

        opts[0].state = state;

        const uint32_t pos_state = position & posMask;

        opts[1].price = Price::RcBit0Price(isMatch[(uint8_t)state][pos_state])
                               + getLiteralPrice(position, buf[-1], !IsLiteralState(state), match_byte, current_byte);

        opts[1].makeLiteral();

        const uint32_t match_price = Price::RcBit1Price(isMatch[(uint8_t)state][pos_state]);
        const uint32_t rep_match_price = match_price + Price::RcBit1Price(isRep[(uint8_t)state]);

        if (match_byte == current_byte)
        {
            const uint32_t short_rep_price = rep_match_price + getShortRepPrice(state, pos_state);

            if (short_rep_price < opts[1].price) {
                opts[1].price = short_rep_price;
                opts[1].makeShortRep();
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

            const uint32_t price = rep_match_price + getPureRepPrice(i, state, pos_state);

            do {
                const uint32_t cur_and_len_price = price + repLenEncoder.getLenPrice(rep_len, pos_state);

                if (cur_and_len_price < opts[rep_len].price) {
                    opts[rep_len].price = cur_and_len_price;
                    opts[rep_len].posPrev = 0;
                    opts[rep_len].backPrev = i;
                    opts[rep_len].prev1IsLiteral = false;
                }
            } while (--rep_len >= 2);
        }


        const uint32_t normal_match_price = match_price
                                            + Price::RcBit0Price(isRep[(uint8_t)state]);

        len = rep_lens[0] >= 2 ? rep_lens[0] + 1 : 2;
        if (len <= len_main) {
            uint32_t i = 0;
            while (len > matches[i].len)
                ++i;

            for(; ; ++len) {
                const uint32_t dist = matches[i].dist;
                const uint32_t cur_and_len_price = normal_match_price
                                                   + getDistLenPrice(dist, len, pos_state);

                if (cur_and_len_price < opts[len].price) {
                    opts[len].price = cur_and_len_price;
                    opts[len].posPrev = 0;
                    opts[len].backPrev = dist + REPS;
                    opts[len].prev1IsLiteral = false;
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

    uint32_t Lzma1Encoder::optimumNormalHelper2(uint32_t *reps, const uint8_t *buf, uint32_t len_end, uint32_t position, const uint32_t cur, const uint32_t nice_len,
                                                const uint32_t buf_avail_full)
    {
        uint32_t matches_count = matches_count;
        uint32_t new_len = longestMatchLength;
        uint32_t pos_prev = opts[cur].posPrev;
        LzmaState lzmaState;

        if (opts[cur].prev1IsLiteral)
        {
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

            UpdateLiteral(lzmaState);

        }
        else
        {
            lzmaState = opts[pos_prev].state;
        }

        if (pos_prev == cur - 1)
        {
            if (opts[cur].isShortRep())
                UpdateShortRep(lzmaState);
            else
                UpdateLiteral(lzmaState);
        }
        else
        {
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
                                                             !IsLiteralState(lzmaState), match_byte, current_byte);

        bool next_is_literal = false;

        if (cur_and_1_price < opts[cur + 1].price) {
            opts[cur + 1].price = cur_and_1_price;
            opts[cur + 1].posPrev = cur;
            opts[cur + 1].makeLiteral();
            next_is_literal = true;
        }

        const uint32_t match_price = cur_price
                                     + Price::RcBit1Price(isMatch[(uint8_t)lzmaState][pos_state]);
        const uint32_t rep_match_price = match_price
                                         + Price::RcBit1Price(isRep[(uint8_t)lzmaState]);

        if (match_byte == current_byte
            && !(opts[cur + 1].posPrev < cur
                 && opts[cur + 1].backPrev == 0))
        {

            const uint32_t short_rep_price = rep_match_price + getShortRepPrice(lzmaState, pos_state);

            if (short_rep_price <= opts[cur + 1].price)
            {
                opts[cur + 1].price = short_rep_price;
                opts[cur + 1].posPrev = cur;
                opts[cur + 1].makeShortRep();
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

            const uint32_t len_test = LzmaMemcmplen(buf, buf_back, 1, limit) - 1;

            if (len_test >= 2)
            {
                LzmaState state_2 = lzmaState;
                UpdateLiteral(state_2);

                const uint32_t pos_state_next = (position + 1) & posMask;
                const uint32_t next_rep_match_price = cur_and_1_price
                                                      + Price::RcBit1Price(isMatch[(uint8_t)state_2][pos_state_next])
                                                      + Price::RcBit1Price(isRep[(uint8_t)state_2]);

                //for (; len_test >= 2; --len_test) {
                const uint32_t offset = cur + 1 + len_test;

                while (len_end < offset)
                    opts[++len_end].price = RC_INFINITY_PRICE;

                const uint32_t cur_and_len_price = next_rep_match_price
                                                   + getRepPrice(0, len_test, state_2, pos_state_next);

                if (cur_and_len_price < opts[offset].price)
                {
                    opts[offset].price = cur_and_len_price;
                    opts[offset].posPrev = cur + 1;
                    opts[offset].backPrev = 0;
                    opts[offset].prev1IsLiteral = true;
                    opts[offset].prev2 = false;
                }
                //}
            }
        }


        uint32_t start_len = 2; // speed optimization

        for (uint32_t rep_index = 0; rep_index < REPS; ++rep_index) {
            const uint8_t *const buf_back = buf - reps[rep_index] - 1;
            if (not_equal_16(buf, buf_back))
                continue;

            uint32_t len_test = LzmaMemcmplen(buf, buf_back, 2, buf_avail);

            while (len_end < cur + len_test)
                opts[++len_end].price = RC_INFINITY_PRICE;

            const uint32_t len_test_temp = len_test;
            const uint32_t price = rep_match_price + getPureRepPrice(rep_index, lzmaState, pos_state);

            do
            {
                const uint32_t cur_and_len_price = price + repLenEncoder.getLenPrice(len_test, pos_state);

                if (cur_and_len_price < opts[cur + len_test].price) {
                    opts[cur + len_test].price = cur_and_len_price;
                    opts[cur + len_test].posPrev = cur;
                    opts[cur + len_test].backPrev = rep_index;
                    opts[cur + len_test].prev1IsLiteral = false;
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
                len_test_2 = LzmaMemcmplen(buf, buf_back, len_test_2, limit);

            len_test_2 -= len_test + 1;

            if (len_test_2 >= 2)
            {
                LzmaState state_2 = lzmaState;
                UpdateLongRep(state_2);

                uint32_t pos_state_next = (position + len_test) & posMask;

                const uint32_t cur_and_len_literal_price = price
                                                           + repLenEncoder.getLenPrice(len_test, pos_state)
                                                           + Price::RcBit0Price(isMatch[(uint8_t)state_2][pos_state_next])
                                                           + getLiteralPrice(position + len_test,
                                                                               buf[len_test - 1], true,
                                                                               buf_back[len_test], buf[len_test]);

                UpdateLiteral(state_2);

                pos_state_next = (position + len_test + 1) & posMask;

                const uint32_t next_rep_match_price = cur_and_len_literal_price
                                                      + Price::RcBit1Price(isMatch[(uint8_t)state_2][pos_state_next])
                                                      + Price::RcBit1Price(isRep[(uint8_t)state_2]);

                const uint32_t offset = cur + len_test + 1 + len_test_2;

                while (len_end < offset)
                    opts[++len_end].price = RC_INFINITY_PRICE;

                const uint32_t cur_and_len_price = next_rep_match_price
                                                   + getRepPrice(0, len_test_2, state_2, pos_state_next);

                if (cur_and_len_price < opts[offset].price)
                {
                    opts[offset].price = cur_and_len_price;
                    opts[offset].posPrev = cur + len_test + 1;
                    opts[offset].backPrev = 0;
                    opts[offset].prev1IsLiteral = true;
                    opts[offset].prev2 = true;
                    opts[offset].posPrev2 = cur;
                    opts[offset].backPrev2 = rep_index;
                }
            }
        }


        if (new_len > buf_avail)
        {
            new_len = buf_avail;

            matches_count = 0;
            while (new_len > matches[matches_count].len)
                ++matches_count;

            matches[matches_count++].len = new_len;
        }


        if (new_len >= start_len)
        {
            const uint32_t normal_match_price = match_price + Price::RcBit0Price(isRep[(uint8_t)lzmaState]);

            while (len_end < cur + new_len)
                opts[++len_end].price = RC_INFINITY_PRICE;

            uint32_t i = 0;
            while (start_len > matches[i].len)
                ++i;

            for (uint32_t len_test = start_len; ; ++len_test)
            {
                const uint32_t cur_back = matches[i].dist;
                uint32_t cur_and_len_price = normal_match_price
                                             + getDistLenPrice(cur_back, len_test, pos_state);

                if (cur_and_len_price < opts[cur + len_test].price) {
                    opts[cur + len_test].price = cur_and_len_price;
                    opts[cur + len_test].posPrev = cur;
                    opts[cur + len_test].backPrev
                            = cur_back + REPS;
                    opts[cur + len_test].prev1IsLiteral = false;
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
                        len_test_2 = LzmaMemcmplen(buf, buf_back, len_test_2, limit);

                    len_test_2 -= len_test + 1;

                    if (len_test_2 >= 2)
                    {
                        LzmaState state_2 = lzmaState;
                        UpdateMatch(state_2);
                        uint32_t pos_state_next
                                = (position + len_test) & posMask;

                        const uint32_t cur_and_len_literal_price = cur_and_len_price
                                                                   + Price::RcBit0Price(isMatch[(uint8_t)state_2][pos_state_next])
                                                                   + getLiteralPrice(position + len_test,
                                                                                       buf[len_test - 1],
                                                                                       true,
                                                                                       buf_back[len_test],
                                                                                       buf[len_test]);

                        UpdateLiteral(state_2);
                        pos_state_next = (pos_state_next + 1) & posMask;

                        const uint32_t next_rep_match_price
                                = cur_and_len_literal_price
                                  + Price::RcBit1Price(isMatch[(uint8_t)state_2][pos_state_next])
                                  + Price::RcBit1Price(isRep[(uint8_t)state_2]);

                        // for(; len_test_2 >= 2; --len_test_2) {
                        const uint32_t offset = cur + len_test + 1 + len_test_2;

                        while (len_end < offset)
                            opts[++len_end].price = RC_INFINITY_PRICE;

                        cur_and_len_price = next_rep_match_price
                                            + getRepPrice(0, len_test_2, state_2, pos_state_next);

                        if (cur_and_len_price < opts[offset].price)
                        {
                            opts[offset].price = cur_and_len_price;
                            opts[offset].posPrev = cur + len_test + 1;
                            opts[offset].backPrev = 0;
                            opts[offset].prev1IsLiteral = true;
                            opts[offset].prev2 = true;
                            opts[offset].posPrev2 = cur;
                            opts[offset].backPrev2 = cur_back + REPS;
                        }

                    }

                    if (++i == matches_count)
                        break;
                }
            }
        }

        return len_end;
    }

    uint32_t Lzma1Encoder::getShortRepPrice(const LzmaState state, const uint32_t pos_state)
    {
        return Price::RcBit0Price(isRep0[(uint8_t)state])
               + Price::RcBit0Price(isRep0Long[(uint8_t)state][pos_state]);
    }

    uint32_t Lzma1Encoder::getPureRepPrice(const uint32_t rep_index, const LzmaState state, uint32_t pos_state)
    {
        uint32_t price;

        if (rep_index == 0) {
            price = Price::RcBit0Price(isRep0[(uint8_t)state]);
            price += Price::RcBit1Price(isRep0Long[(uint8_t)state][pos_state]);
        } else {
            price = Price::RcBit1Price(isRep0[(uint8_t)state]);

            if (rep_index == 1) {
                price += Price::RcBit0Price(isRep1[(uint8_t)state]);
            } else {
                price += Price::RcBit1Price(isRep1[(uint8_t)state]);
                price += Price::RcBitPrice(isRep2[(uint8_t)state],
                                      rep_index - 2);
            }
        }

        return price;
    }

    uint32_t Lzma1Encoder::getRepPrice(const uint32_t rep_index, const uint32_t len, const LzmaState state, const uint32_t pos_state)
    {
        return repLenEncoder.getLenPrice(len, pos_state)
               + getPureRepPrice(rep_index, state, pos_state);
    }

    uint32_t Lzma1Encoder::getDistLenPrice(const uint32_t dist, const uint32_t len, const uint32_t pos_state)
    {

        const uint32_t dist_state = GetDistState(len);
        uint32_t price;

        if (dist < FULL_DISTANCES) {
            price = distPrices[dist_state][dist];
        } else {
            const uint32_t dist_slot = GetDistSlot2(dist);
            price = distSlotPrices[dist_state][dist_slot]
                    + alignPrices[dist & ALIGN_MASK];
        }

        price += matchLenEncoder.getLenPrice(len, pos_state);

        return price;
    }

    void Lzma1Encoder::backward(uint32_t *len_res, uint32_t *back_res, uint32_t cur)
    {
        optsEndIndex = cur;

        uint32_t pos_mem = opts[cur].posPrev;
        uint32_t back_mem = opts[cur].backPrev;

        do
        {
            if (opts[cur].prev1IsLiteral)
            {
                opts[pos_mem].makeLiteral();
                opts[pos_mem].posPrev = pos_mem - 1;

                if (opts[cur].prev2)
                {
                    opts[pos_mem - 1].prev1IsLiteral = false;
                    opts[pos_mem - 1].posPrev = opts[cur].posPrev2;
                    opts[pos_mem - 1].backPrev = opts[cur].backPrev2;
                }
            }

            const uint32_t pos_prev = pos_mem;
            const uint32_t back_cur = back_mem;

            back_mem = opts[pos_prev].backPrev;
            pos_mem = opts[pos_prev].posPrev;

            opts[pos_prev].backPrev = back_cur;
            opts[pos_prev].posPrev = cur;
            cur = pos_prev;

        } while (cur != 0);

        optsCurrentIndex = opts[0].posPrev;
        *len_res = opts[0].posPrev;
        *back_res = opts[0].backPrev;

        return;
    }

    void Lzma1Encoder::encodeSymbol(LzmaMF *mf, uint32_t back, uint32_t len, uint32_t position)
    {
        const uint32_t pos_state = position & posMask;

        if (back == UINT32_MAX)
        {
            // Literal i.e. eight-bit byte
            assert(len == 1);
            rc.bit( &isMatch[(uint8_t)state][pos_state], 0);
            literalFunc(mf, position);
        }
        else
        {
            // Some type of match
            rc.bit(&isMatch[(uint8_t)state][pos_state], 1);

            if (back < REPS)
            {
                // It's a repeated match i.e. the same distance
                // has been used earlier.
                rc.bit(&isRep[(uint8_t)state], 1);
                repMatch(pos_state, back, len);
            }
            else
            {
                // Normal match
                rc.bit(&isRep[(uint8_t)state], 0);
                match(pos_state, back - REPS, len);
            }
        }

        assert(mf->readAhead >= len);
        mf->readAhead -= len;
    }

    void Lzma1Encoder::literalFunc(LzmaMF *mf, uint32_t position)
    {
        // Locate the literal byte to be encoded and the subcoder.
        const uint8_t cur_byte = mf->buffer[
                mf->readPos - mf->readAhead];
        probability *subcoder = literal_subcoder(literal,
                                                 literalContextBits, literalPosMask,
                                                 position, mf->buffer[mf->readPos - mf->readAhead - 1]);

        if (IsLiteralState(state))
        {
            // Previous LZMA-symbol was a literal. Encode a normal
            // literal without a match byte.
            rc.bittree(subcoder, 8, cur_byte);
        }
        else
        {
            // Previous LZMA-symbol was a match. Use the last byte of
            // the match as a "match byte". That is, compare the bits
            // of the current literal and the match byte.
            const uint8_t match_byte = mf->buffer[
                    mf->readPos - reps[0] - 1
                    - mf->readAhead];
            rc.literalMatched(subcoder, match_byte, cur_byte);
        }

        UpdateLiteral(state);
    }

    void Lzma1Encoder::match(const uint32_t pos_state, const uint32_t distance, const uint32_t len)
    {
        UpdateMatch(state);

        rc.length(&matchLenEncoder, pos_state, len, fastMode);

        const uint32_t dist_slot = GetDistSlot(distance);
        const uint32_t dist_state = GetDistState(len);
        rc.bittree(distSlot[dist_state], DIST_SLOT_BITS, dist_slot);

        if (dist_slot >= DIST_MODEL_START) {
            const uint32_t footer_bits = (dist_slot >> 1) - 1;
            const uint32_t base = (2 | (dist_slot & 1)) << footer_bits;
            const uint32_t dist_reduced = distance - base;

            if (dist_slot < DIST_MODEL_END)
            {
                // Careful here: base - dist_slot - 1 can be -1, but
                // rc_bittree_reverse starts at probs[1], not probs[0].
                rc.bittreeReverse(distSpecial + base - dist_slot - 1, footer_bits, dist_reduced);
            }
            else
            {
                rc.direct(dist_reduced >> ALIGN_BITS, footer_bits - ALIGN_BITS);
                rc.bittreeReverse(distAlign, ALIGN_BITS, dist_reduced & ALIGN_MASK);
                ++alignPriceCount;
            }
        }

        reps[3] = reps[2];
        reps[2] = reps[1];
        reps[1] = reps[0];
        reps[0] = distance;
        ++matchPriceCount;
    }

    void Lzma1Encoder::repMatch(const uint32_t pos_state, const uint32_t rep, const uint32_t len)
    {
        if (rep == 0) {
            rc.bit(&isRep0[(uint8_t)state], 0);
            rc.bit(&isRep0Long[(uint8_t)state][pos_state],
                   len != 1);
        } else {
            const uint32_t distance = reps[rep];
            rc.bit( &isRep0[(uint8_t)state], 1);

            if (rep == 1)
            {
                rc.bit(&isRep1[(uint8_t)state], 0);
            }
            else
            {
                rc.bit(&isRep1[(uint8_t)state], 1);
                rc.bit(&isRep2[(uint8_t)state], rep - 2);

                if (rep == 3)
                    reps[3] = reps[2];

                reps[2] = reps[1];
            }

            reps[1] = reps[0];
            reps[0] = distance;
        }

        if (len == 1)
        {
            UpdateShortRep(state);
        }
        else
        {
            rc.length(&repLenEncoder, pos_state, len, fastMode);
            UpdateLongRep(state);
        }
    }

    void Lzma1Encoder::encodeEopm(uint32_t position)
    {
        const uint32_t pos_state = position & posMask;
        rc.bit(&isMatch[(uint8_t)state][pos_state], 1);
        rc.bit(&isRep[(uint8_t)state], 0);
        match(pos_state, UINT32_MAX, MATCH_LEN_MIN);
    }

    StatusCode Lzma1Encoder::reset(const LzmaOptions *options)
    {

        if (!options->isValid())
            return StatusCode::OptionsError;

        posMask = (1U << options->pb) - 1;
        literalContextBits = options->lc;
        literalPosMask = (1U << options->lp) - 1;

        // Range coder
        rc.reset();

        // State
        state = LzmaState::LitLit;
        for (size_t i = 0; i < REPS; ++i)
            reps[i] = 0;

        LiteralInit(literal, options->lc, options->lp);

        // Bit encoders
        for (size_t i = 0; i < STATES; ++i) {
            for (size_t j = 0; j <= posMask; ++j) {
                BitReset(isMatch[i][j]);
                BitReset(isRep0Long[i][j]);
            }

            BitReset(isRep[i]);
            BitReset(isRep0[i]);
            BitReset(isRep1[i]);
            BitReset(isRep2[i]);
        }

        for (size_t i = 0; i < FULL_DISTANCES - DIST_MODEL_END; ++i)
            BitReset(distSpecial[i]);

        // Bit tree encoders
        for (size_t i = 0; i < DIST_STATES; ++i)
            BittreeReset(distSlot[i], DIST_SLOT_BITS);

        BittreeReset(distAlign, ALIGN_BITS);

        // Length encoders
        matchLenEncoder.reset(1U << options->pb, fastMode);
        repLenEncoder.reset(1U << options->pb, fastMode);

        // Price counts are incremented every time appropriate probabilities
        // are changed. price counts are set to zero when the price tables
        // are updated, which is done when the appropriate price counts have
        // big enough value, and lzma_mf.read_ahead == 0 which happens at
        // least every OPTS (a few thousand) possible price count increments.
        //
        // By resetting price counts to UINT32_MAX / 2, we make sure that the
        // price tables will be initialized before they will be used (since
        // the value is definitely big enough), and that it is OK to increment
        // price counts without risk of integer overflow (since UINT32_MAX / 2
        // is small enough). The current code doesn't increment price counts
        // before initializing price tables, but it maybe done in future if
        // we add support for saving the state between LZMA2 chunks.
        matchPriceCount = UINT32_MAX / 2;
        alignPriceCount = UINT32_MAX / 2;

        optsEndIndex = 0;
        optsCurrentIndex = 0;

        return StatusCode::Ok;
    }

    StatusCode Lzma1Encoder::create(const LzmaOptions *options, LzOptions *lz_options)
    {


        // Set compression mode. We haven't validates the options yet,
        // but it's OK here, since nothing bad happens with invalid
        // options in the code below, and they will get rejected by
        // lzma_lzma_encoder_reset() call at the end of this function.
        switch (options->mode) {
            case Mode::Fast:
                fastMode = true;
                break;

            case Mode::Normal:
            {
                fastMode = false;

                // Set dist_table_size.
                // Round the dictionary size up to next 2^n.
                uint32_t log_size = 0;
                while ((1 << log_size) < options->dictSize)
                    ++log_size;

                distTableSize = log_size * 2;

                // Length encoders' price table size
                matchLenEncoder.tableSize = options->niceLen + 1 - MATCH_LEN_MIN;
                repLenEncoder.tableSize = options->niceLen + 1 - MATCH_LEN_MIN;
                break;
            }

            default:
                return StatusCode::OptionsError;
        }

        // We don't need to write the first byte as literal if there is
        // a non-empty preset dictionary. encode_init() wouldn't even work
        // if there is a non-empty preset dictionary, because encode_init()
        // assumes that position is zero and previous byte is also zero.
        isInitialized = options->presetDict != NULL
                                && options->presetDictSize > 0;
        isFlushed = false;

        lz_options->setOptions(*options);

        return reset(options);
    }
}

#endif //POCKETLZMA_LZMA1ENCODER_HPP
