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
            inline StatusCode encode(Lzma2Coder *coder, LzmaMF * mf, uint8_t * out, size_t * out_pos,
                                     size_t out_size);
            /*!
             * Validates the coder then assigns header data.
             * Based on lzma2_header_lzma
             * @param coder
             * @return
             */
            inline StatusCode validateCoderAndAssignHeader(Lzma2Coder *coder);
            /*! Based on lzma2_header_uncompressed */
            inline StatusCode headerUncompressed(Lzma2Coder *coder);

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

    StatusCode Lzma2Compressor::encode(Lzma2Coder *coder, LzmaMF *mf, uint8_t *out, size_t *out_pos, size_t out_size)
    {
        while (*out_pos < out_size)
        {
            switch (coder->sequence)
            {
                case Lzma2Coder::Sequence::Init:
                    // If there's no input left and we are flushing or finishing,
                    // don't start a new chunk.
                    if (mf->unencoded() == 0) //(MfUnencoded(mf) == 0)
                    {
                        // Write end of payload marker if finishing.
                        if (mf->action == LzmaAction::Finish)
                            out[(*out_pos)++] = 0;

                        return (mf->action == LzmaAction::Run) ? StatusCode::Ok : StatusCode::StreamEnd;
                    }

                    if (coder->needStateReset)
                        return_if_error(coder->lzma->reset(&coder->optCur));

                    coder->uncompressedSize = 0;
                    coder->compressedSize = 0;
                    coder->sequence = Lzma2Coder::Sequence::Encode;

                    // Fall through

                case Lzma2Coder::Sequence::Encode:
                {
                    // Calculate how much more uncompressed data this chunk
                    // could accept.
                    const uint32_t left = LZMA2_MAX_UNCOMPRESSED_SIZE
                                          - coder->uncompressedSize;
                    uint32_t limit;

                    if (left < mf->matchLenMax)
                    {
                        // Must flush immediately since the next LZMA symbol
                        // could make the uncompressed size of the chunk too
                        // big.
                        limit = 0;
                    }
                    else
                    {
                        // Calculate maximum read_limit that is OK from point
                        // of view of LZMA2 chunk size.
                        limit = mf->readPos - mf->readAhead
                                + left - mf->matchLenMax;
                    }

                    // Save the start position so that we can update
                    // coder->uncompressed_size.
                    const uint32_t read_start = mf->readPos - mf->readAhead;

                    // Call the LZMA encoder until the chunk is finished.
                    const StatusCode ret = LzmaCompressor::lzmaEncode(coder->lzma.get(), mf,
                                                          coder->buf + LZMA2_MAX_HEADER_SIZE,
                                                          &coder->compressedSize,
                                                          LZMA2_MAX_CHUNK_SIZE, limit);

                    coder->uncompressedSize += mf->readPos - mf->readAhead
                                               - read_start;

                    assert(coder->compressedSize <= LZMA2_MAX_CHUNK_SIZE);
                    assert(coder->uncompressedSize <= LZMA2_MAX_UNCOMPRESSED_SIZE);

                    if (ret != StatusCode::StreamEnd)
                        return StatusCode::Ok;

                    // See if the chunk compressed. If it didn't, we encode it
                    // as uncompressed chunk. This saves a few bytes of space
                    // and makes decoding faster.
                    if (coder->compressedSize >= coder->uncompressedSize)
                    {
                        coder->uncompressedSize += mf->readAhead;
                        assert(coder->uncompressedSize
                               <= LZMA2_MAX_UNCOMPRESSED_SIZE);
                        mf->readAhead = 0;
                        headerUncompressed(coder);
                        coder->needStateReset = true;
                        coder->sequence = Lzma2Coder::Sequence::UncompressedHeader;
                        break;
                    }

                    // The chunk did compress at least by one byte, so we store
                    // the chunk as LZMA.
                    validateCoderAndAssignHeader(coder); // lzma2_header_lzma(coder);

                    coder->sequence = Lzma2Coder::Sequence::Copy;
                }

                    // Fall through

                case Lzma2Coder::Sequence::Copy:
                    // Copy the compressed chunk along its headers to the
                    // output buffer.
                    LzmaBufcpy(coder->buf, &coder->bufPos, coder->compressedSize, out, out_pos, out_size);

                    if (coder->bufPos != coder->compressedSize)
                        return StatusCode::Ok;

                    coder->sequence = Lzma2Coder::Sequence::Init;
                    break;

                case Lzma2Coder::Sequence::UncompressedHeader:
                    // Copy the three-byte header to indicate uncompressed chunk.
                    LzmaBufcpy(coder->buf, &coder->bufPos, LZMA2_HEADER_SIZE_UNCOMPRESSED, out, out_pos, out_size);
                    if (coder->bufPos != LZMA2_HEADER_SIZE_UNCOMPRESSED)
                        return StatusCode::Ok;

                    coder->sequence = Lzma2Coder::Sequence::UncompressedCopy;

                    // Fall through

                case Lzma2Coder::Sequence::UncompressedCopy:
                    // Copy the uncompressed data as is from the dictionary
                    // to the output buffer.
                    mf->read(out, out_pos, out_size, &coder->uncompressedSize);
                    if (coder->uncompressedSize != 0)
                        return StatusCode::Ok;

                    coder->sequence = Lzma2Coder::Sequence::Init;
                    break;
            }
        }

        return StatusCode::Ok;
    }

    StatusCode Lzma2Compressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {

        return StatusCode::UndefinedError;
    }

    StatusCode Lzma2Compressor::headerUncompressed(Lzma2Coder *coder)
    {
        if (coder->uncompressedSize == 0) return StatusCode::UncompressedSizeIsZero;
        if (coder->uncompressedSize > LZMA2_MAX_CHUNK_SIZE) return StatusCode::UncompressedSizeTooLargeLzma2;

        // If this is the first chunk, we need to include dictionary
        // reset indicator.
        if (coder->needDictionaryReset)
            coder->buf[0] = 1;
        else
            coder->buf[0] = 2;

        coder->needDictionaryReset = false;

        // "Compressed" size
        coder->buf[1] = (coder->uncompressedSize - 1) >> 8;
        coder->buf[2] = (coder->uncompressedSize - 1) & 0xFF;

        // Set the start position for copying.
        coder->bufPos = 0;

        return StatusCode::Ok;
    }



}

#endif //POCKETLZMA_LZMA2COMPRESSOR_HPP
