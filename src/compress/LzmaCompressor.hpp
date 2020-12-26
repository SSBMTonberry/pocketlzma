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
