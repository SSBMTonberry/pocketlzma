//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA2COMPRESSOR_HPP
#define POCKETLZMA_LZMA2COMPRESSOR_HPP

namespace plz
{
    class Lzma2Compressor : public LzmaCompressor
    {
        public:
            Lzma2Compressor() = default;

            inline StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) override;

        private:
            /*!
             * Validates the coder then assigns header data.
             * Based on lzma2_header_lzma
             * @param coder
             * @return
             */
            inline StatusCode validateCoderAndAssignHeader(Lzma2Coder *coder);


            uint32_t m_bufferSize {LZMA2_MAX_UNCOMPRESSED_SIZE};
            uint32_t m_chunkSize {LZMA2_MAX_CHUNK_SIZE};
    };


    StatusCode Lzma2Compressor::validateCoderAndAssignHeader(Lzma2Coder *coder)
    {
        if(coder->uncompressedSize == 0) return StatusCode::UncompressedSizeIsZero;
        if(coder->uncompressedSize > LZMA2_MAX_UNCOMPRESSED_SIZE) return StatusCode::UncompressedSizeTooLargeLzma2;
        if(coder->compressedSize == 0) return StatusCode::CompressedSizeIsZero;
        if(coder->compressedSize > LZMA2_MAX_CHUNK_SIZE) return StatusCode::CompressedSizeIsLargerThanMaxChunkLzma2;

        size_t pos;

        if (coder->needProperties) {
            pos = 0;

            if (coder->needDictionaryReset)
                coder->buf[pos] = 0x80 + (3 << 5);
            else
                coder->buf[pos] = 0x80 + (2 << 5);
        } else {
            pos = 1;

            if (coder->needStateReset)
                coder->buf[pos] = 0x80 + (1 << 5);
            else
                coder->buf[pos] = 0x80;
        }

        // Set the start position for copying.
        coder->bufPos = pos;

        // Uncompressed size
        size_t size = coder->uncompressedSize - 1;
        coder->buf[pos++] += size >> 16;
        coder->buf[pos++] = (size >> 8) & 0xFF;
        coder->buf[pos++] = size & 0xFF;

        // Compressed size
        size = coder->compressedSize - 1;
        coder->buf[pos++] = size >> 8;
        coder->buf[pos++] = size & 0xFF;

        // Properties, if needed
        if (coder->needProperties)
        {
            StatusCode status = lzmaLclppbEncode(&coder->optCur, coder->buf + pos);
            if(status != StatusCode::Ok)
                return status;
        }

        coder->needProperties = false;
        coder->needStateReset = false;
        coder->needDictionaryReset = false;

        // The copying code uses coder->compressed_size to indicate the end
        // of coder->buf[], so we need add the maximum size of the header here.
        coder->compressedSize += LZMA2_MAX_HEADER_SIZE;

        return StatusCode::Ok;
    }

    StatusCode Lzma2Compressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {

        return StatusCode::UndefinedError;
    }

}

#endif //POCKETLZMA_LZMA2COMPRESSOR_HPP
