//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA2CODER_HPP
#define POCKETLZMA_LZMA2CODER_HPP

namespace plz
{
    class Lzma2Coder
    {
        public:
            Lzma2Coder() = default;

            enum class Sequence : uint8_t
            {
                Init = 0,
                LzmaEncode = 1,
                LzmaCopy = 2,
                UncompressedHeader = 3,
                UncompressedCopy = 4
            };

            /*! LZMA encoder */
            void *lzma;

            /// LZMA options currently in use.
            lzma_options_lzma opt_cur;

            bool needProperties;
            bool needStateReset;
            bool needDictionaryReset;

            /*! Uncompressed size of a chunk */
            size_t uncompressedSize;

            /*!
             * Compressed size of a chunk (excluding headers); this is also used
             * to indicate the end of buf[] in SEQ_LZMA_COPY.
             */
            size_t compressedSize;

            /*! Read position in buf[] */
            size_t bufPos;

            /*! Buffer to hold the chunk header and LZMA compressed data */
            uint8_t buf[LZMA2_MAX_HEADER_SIZE + LZMA2_MAX_CHUNK_SIZE];
    };
}
#endif //POCKETLZMA_LZMA2CODER_HPP
