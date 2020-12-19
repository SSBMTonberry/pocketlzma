//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA2COMPRESSOR_HPP
#define POCKETLZMA_LZMA2COMPRESSOR_HPP

namespace plz
{
    class Lzma2Compressor : public ICompressor
    {
        public:
            Lzma2Compressor() = default;

            inline StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) override;

        private:
            uint32_t m_bufferSize {LZMA2_MAX_BUFFER_SIZE};
            uint32_t m_chunkSize {LZMA2_MAX_CHUNK_SIZE};
    };

    StatusCode Lzma2Compressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {

        return StatusCode::UndefinedError;
    }
}

#endif //POCKETLZMA_LZMA2COMPRESSOR_HPP
