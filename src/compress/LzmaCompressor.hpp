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
    };

    StatusCode LzmaCompressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {
        return StatusCode::UndefinedError;
    }

    StatusCode LzmaCompressor::lzmaLclppbEncode(const LzmaOptions *options, uint8_t *byte)
    {
        StatusCode lclppbStatus = IsLclppbValid(options);
        if (lclppbStatus != StatusCode::Ok)
            return lclppbStatus;

        *byte = (options->pb * 5 + options->lp) * 9 + options->lc;
        if(*byte > (4 * 5 + 4) * 9 + 8)
            return StatusCode::InvalidLclppbByteValue;

        return StatusCode::Ok;
    }
}
#endif //POCKETLZMA_LZMACOMPRESSOR_HPP
