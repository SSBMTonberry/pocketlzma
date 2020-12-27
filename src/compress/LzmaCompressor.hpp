//
// Created by robin on 24.12.2020.
//

#ifndef POCKETLZMA_LZMACOMPRESSOR_HPP
#define POCKETLZMA_LZMACOMPRESSOR_HPP

namespace plz
{
    class LzmaCompressor : public ICompressor
    {
        public:
            LzmaCompressor() = default;
            inline StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) override;

        protected:
            inline StatusCode encode(Lzma1Encoder *coder, LzmaMF *mf,
                                     uint8_t *out, size_t *out_pos,
                                     size_t out_size);
            inline StatusCode lzmaEncode(Lzma1Encoder * coder, LzmaMF * mf,
                             uint8_t * out, size_t * out_pos,
                             size_t out_size, uint32_t limit);

            /*! Encodes lc/lp/pb into one byte. */
            inline StatusCode lzmaLclppbEncode(const LzmaOptions *options, uint8_t *byte);
            inline StatusCode lzmaEncoderReset(Lzma1Encoder *coder, const LzmaOptions *options);
            inline bool isOptionsValid(const LzmaOptions *options);
            inline StatusCode literalInit(probability (*probs)[LITERAL_CODER_SIZE], uint32_t lc, uint32_t lp);
    };

    StatusCode LzmaCompressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {

        return StatusCode::UndefinedError;
    }

    StatusCode LzmaCompressor::encode(Lzma1Encoder *coder, LzmaMF *mf, uint8_t *out, size_t *out_pos, size_t out_size)
    {
        // Plain LZMA has no support for sync-flushing.
        if (unlikely(mf->action == LzmaAction::SyncFlush))
            return StatusCode::OptionsError;
        return lzmaEncode(coder, mf, out, out_pos, out_size, UINT32_MAX);
    }

    StatusCode LzmaCompressor::lzmaEncode(Lzma1Encoder *coder, LzmaMF *mf, uint8_t *out, size_t *out_pos, size_t out_size, uint32_t limit)
    {
        // Initialize the stream if no data has been encoded yet.
        if (!coder->isInitialized && !coder->init(mf))
            return StatusCode::Ok;

        // Get the lowest bits of the uncompressed offset from the LZ layer.
        uint32_t position = mf->position();

        while (true)
        {
            // Encode pending bits, if any. Calling this before encoding
            // the next symbol is needed only with plain LZMA, since
            // LZMA2 always provides big enough buffer to flush
            // everything out from the range encoder. For the same reason,
            // rc_encode() never returns true when this function is used
            // as part of LZMA2 encoder.
            if (coder->rc.encode(out, out_pos, out_size))
            {
                assert(limit == UINT32_MAX);
                return StatusCode::Ok;
            }

            // With LZMA2 we need to take care that compressed size of
            // a chunk doesn't get too big.
            // FIXME? Check if this could be improved.
            if (limit != UINT32_MAX
                && (mf->readPos - mf->readAhead >= limit ||
                    *out_pos + coder->rc.pending() >= LZMA2_MAX_CHUNK_SIZE - LOOP_INPUT_MAX))
            {
                break;
            }

            // Check that there is some input to process.
            if (mf->readPos >= mf->readLimit)
            {
                if (mf->action == LzmaAction::Run)
                    return StatusCode::Ok;

                if (mf->readAhead == 0)
                    break;
            }

            // Get optimal match (repeat position and length).
            // Value ranges for pos:
            //   - [0, REPS): repeated match
            //   - [REPS, UINT32_MAX):
            //     match at (pos - REPS)
            //   - UINT32_MAX: not a match but a literal
            // Value ranges for len:
            //   - [MATCH_LEN_MIN, MATCH_LEN_MAX]
            uint32_t len;
            uint32_t back;

            if (coder->fastMode)
                coder->lzmaOptimumFast(mf, &back, &len);
            else
                coder->lzmaOptimumNormal(mf, &back, &len, position);

            coder->encodeSymbol(mf, back, len, position);

            position += len;
        }

        if (!coder->isFlushed)
        {
            coder->isFlushed = true;

            // We don't support encoding plain LZMA streams without EOPM,
            // and LZMA2 doesn't use EOPM at LZMA level.
            if (limit == UINT32_MAX)
                encode_eopm(coder, position);

            // Flush the remaining bytes from the range encoder.
            coder->rc.flush();

            // Copy the remaining bytes to the output buffer. If there
            // isn't enough output space, we will copy out the remaining
            // bytes on the next call to this function by using
            // the rc_encode() call in the encoding loop above.
            if (coder->rc.encode(out, out_pos, out_size))
            {
                assert(limit == UINT32_MAX);
                return StatusCode::Ok;
            }
        }

        // Make it ready for the next LZMA2 chunk.
        coder->isFlushed = false;

        return StatusCode::StreamEnd;
    }

    StatusCode LzmaCompressor::lzmaLclppbEncode(const LzmaOptions *options, uint8_t *byte)
    {
        StatusCode lclppbStatus = options->isLclppbValid() ? StatusCode::Ok : StatusCode::ErrorInLclppbCheckOnLzmaOptions;
        if (lclppbStatus != StatusCode::Ok)
            return lclppbStatus;

        *byte = (options->pb * 5 + options->lp) * 9 + options->lc;
        if(*byte > (4 * 5 + 4) * 9 + 8)
            return StatusCode::InvalidLclppbByteValue;

        return StatusCode::Ok;
    }

    StatusCode LzmaCompressor::lzmaEncoderReset(Lzma1Encoder *coder, const LzmaOptions *options)
    {
        if (!isOptionsValid(options))
            return StatusCode::OptionsError;

        coder->posMask = (1U << options->pb) - 1;
        coder->literalContextBits = options->lc;
        coder->literalPosMask = (1U << options->lp) - 1;

        // Range coder
        coder->rc.reset();

        // State
        coder->state = LzmaState::LitLit;
        for (size_t i = 0; i < REPS; ++i)
            coder->reps[i] = 0;

        StatusCode initStatus = literalInit(coder->literal, options->lc, options->lp);
        if(initStatus != StatusCode::Ok)
            return initStatus;

        // Bit encoders
        for (size_t i = 0; i < STATES; ++i) {
            for (size_t j = 0; j <= coder->posMask; ++j) {
                BitReset(coder->isMatch[i][j]);
                BitReset(coder->isRep0Long[i][j]);
            }

            BitReset(coder->isRep[i]);
            BitReset(coder->isRep0[i]);
            BitReset(coder->isRep1[i]);
            BitReset(coder->isRep2[i]);
        }

        for (size_t i = 0; i < FULL_DISTANCES - DIST_MODEL_END; ++i)
            BitReset(coder->distSpecial[i]);

        // Bit tree encoders
        for (size_t i = 0; i < DIST_STATES; ++i)
            BittreeReset(coder->distSlot[i], DIST_SLOT_BITS);

        BittreeReset(coder->distAlign, ALIGN_BITS);

        // Length encoders
        coder->matchLenEncoder.reset(1U << options->pb, coder->fastMode);
        coder->repLenEncoder.reset(1U << options->pb, coder->fastMode);
        // length_encoder_reset(&coder->matchLenEncoder,
        //                      1U << options->pb, coder->fastMode);
//
        // length_encoder_reset(&coder->repLenEncoder,
        //                      1U << options->pb, coder->fastMode);

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
        coder->match_price_count = UINT32_MAX / 2;
        coder->align_price_count = UINT32_MAX / 2;

        coder->opts_end_index = 0;
        coder->opts_current_index = 0;
        return StatusCode::Ok;
    }

    bool LzmaCompressor::isOptionsValid(const LzmaOptions *options)
    {
        // Validate some of the options. LZ encoder validates nice_len too
        // but we need a valid value here earlier.
        return options->isLclppbValid()
               && options->niceLen >= MATCH_LEN_MIN
               && options->niceLen <= MATCH_LEN_MAX
               && (options->mode == Mode::Fast
                   || options->mode == Mode::Normal);
    }

    StatusCode LzmaCompressor::literalInit(probability (*probs)[LITERAL_CODER_SIZE], uint32_t lc, uint32_t lp)
    {
        if(lc + lp > LZMA_LCLP_MAX)
            return StatusCode::LzmaEncodingLiteralInitError;

        const uint32_t coders = 1U << (lc + lp);

        for (uint32_t i = 0; i < coders; ++i)
            for (uint32_t j = 0; j < LITERAL_CODER_SIZE; ++j)
                BitReset(probs[i][j]);

        return StatusCode::Ok;
    }




}
#endif //POCKETLZMA_LZMACOMPRESSOR_HPP
