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
                Encode = 1,
                Copy = 2,
                UncompressedHeader = 3,
                UncompressedCopy = 4
            };

            Sequence sequence {Sequence::Init};
            /*! LZMA encoder */
            std::unique_ptr<Lzma1Encoder> lzma; //RBP: Make this unique_ptr - changed from void* to Lzma1Encoder

            /*! LZMA options currently in use. */
            LzmaOptions optCur;

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

            /*! lzma2_encoder_init - (lzma_lz_encoder *lz, const lzma_allocator *allocator, const void *options, lzma_lz_options *lz_options)*/
            StatusCode init(const LzmaOptions &options, LzOptions &lzOptions, LzmaMF *mf);
    };

    StatusCode Lzma2Coder::init(const LzmaOptions &options, LzOptions &lzOptions, LzmaMF *mf)
    {
        lzma = std::unique_ptr<Lzma1Encoder>(new plz::Lzma1Encoder());
        lzma->init(mf);

        optCur = options;

        sequence = Sequence::Init;
        needProperties = true;
        needStateReset = false;
        needDictionaryReset = optCur.presetDict == nullptr
                  || optCur.presetDictSize == 0;

        // Initialize LZMA encoder
        //return_if_error(lzma_lzma_encoder_create(&lzma, allocator,
        //                                         &optCur, lzOptions));
        return_if_error(lzma->create(&optCur, &lzOptions));

        // Make sure that we will always have enough history available in
        // case we need to use uncompressed chunks. They are used when the
        // compressed size of a chunk is not smaller than the uncompressed
        // size, so we need to have at least LZMA2_COMPRESSED_MAX bytes
        // history available.
        if (lzOptions.beforeSize + lzOptions.dictSize < LZMA2_MAX_CHUNK_SIZE)
            lzOptions.beforeSize
                    = LZMA2_MAX_CHUNK_SIZE - lzOptions.dictSize;

        return StatusCode::Ok;
    }
}
#endif //POCKETLZMA_LZMA2CODER_HPP