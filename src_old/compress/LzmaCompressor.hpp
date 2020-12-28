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
                coder->encodeEopm(position);

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
