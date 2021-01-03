//
// Created by robin on 29.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_CLASS_HPP
#define POCKETLZMA_POCKETLZMA_CLASS_HPP

namespace plz
{
    class PocketLzma
    {
        public:
            PocketLzma() = default;
            inline explicit PocketLzma(Preset preset);
            inline explicit PocketLzma(const Settings &settings) : m_settings {settings} {};
            inline void setSettings(const Settings &settings);
            inline void usePreset (Preset preset);

            inline StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output);
            inline StatusCode compress(const uint8_t *input, const size_t inputSize, std::vector<uint8_t> &output);

            inline StatusCode decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output);
            inline StatusCode decompress(const uint8_t *input, const size_t inputSize, std::vector<uint8_t> &output);

            inline StatusCode decompressBuffered(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint32_t bufferSize = PLZ_BUFFER_SIZE);
            inline StatusCode decompressBuffered(const uint8_t *input, const size_t inputSize, std::vector<uint8_t> &output, uint32_t bufferSize = PLZ_BUFFER_SIZE);
        private:
            Settings m_settings {};
    };

    PocketLzma::PocketLzma(Preset preset)
    {
        usePreset(preset);
    }

    /*!
     * This is optional.
     * PocketLzma uses default values if not set by the user.
     *
     * If you are a casual user: usePreset() is recommended.
     *
     * @param settings new settings
     */
    void PocketLzma::setSettings(const Settings &settings)
    {
        m_settings = settings;
    }

    /*!
     * Set a preset to control compression speed vs ratio.
     *
     * Recommended:
     * For fast compression:     Preset::Fast
     * For balanced compression: Preset::Default
     * For good compression:     Preset::GoodCompression
     *
     * Note: Not used when decompressing.
     */
    void PocketLzma::usePreset(Preset preset)
    {
        m_settings.usePreset(preset);
    }

    StatusCode PocketLzma::compress(const std::vector <uint8_t> &input, std::vector <uint8_t> &output)
    {
        return compress(&input[0], input.size(), output);
    }

    StatusCode PocketLzma::compress(const uint8_t *input, const size_t inputSize, std::vector<uint8_t> &output)
    {
        m_settings.validate();
        size_t propsSize = LZMA_PROPS_SIZE;
        uint8_t propsEncoded[LZMA_PROPS_SIZE];

        size_t outSize = inputSize + (inputSize / 3) + 128;
        //uint8_t out[outSize];
        std::unique_ptr<uint8_t[]> out(new uint8_t[outSize]);

        int rc = plz::c::LzmaCompress(&out[0], &outSize, input, inputSize, propsEncoded, &propsSize, m_settings.level, m_settings.dictionarySize,
                                      m_settings.literalContextBits,m_settings.literalPositionBits,m_settings.positionBits,m_settings.fastBytes,1);

        StatusCode status = static_cast<StatusCode>(rc);
        if(status == StatusCode::Ok)
        {
            std::vector<uint8_t> sizeBits;
            for (int i = 0; i < 8; i++)
                sizeBits.push_back((inputSize >> (i * 8)) & 0xFF);

            output.insert(output.end(), propsEncoded, propsEncoded + propsSize); // Property header
            output.insert(output.end(), sizeBits.begin(), sizeBits.end());           // Add decompress size information
            output.insert(output.end(), out.get(), out.get() + outSize);                     // Data
        }

        return status;
    }

    /*!
     * Decompresses LZMA data.
     *
     * Will choose the best solution based on whether lzma file size in header is known or not.
     * In cases where the file size is unknown, decompressBuffered() will be called
     *
     * @param input The input compressed data
     * @param output The decompressed data
     * @return Status for the decompression process
     */
    StatusCode PocketLzma::decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {
        return decompress(&input[0], input.size(), output);
    }

    /*!
     * Decompresses LZMA data.
     *
     * Will choose the best solution based on whether lzma file size in header is known or not.
     * In cases where the file size is unknown, decompressBuffered() will be called
     *
     * @param input The input compressed data
     * @param output The decompressed data
     * @return Status for the decompression process
     */
    StatusCode PocketLzma::decompress(const uint8_t *input, const size_t inputSize, std::vector<uint8_t> &output)
    {
        if(inputSize <= PLZ_MINIMUM_LZMA_SIZE)
            return StatusCode::InvalidLzmaData;

        size_t propsSize = LZMA_PROPS_SIZE + 8; //header + decompress_size
        size_t size = 0;
        bool sizeInfoMissing = true; //True until proven otherwise

        for (int i = 0; i < 8; i++)
        {
            uint8_t value = input[LZMA_PROPS_SIZE + i];
            if(value != 0xFF)
                sizeInfoMissing = false;

            size |= (value << (i * 8));
        }

        if(sizeInfoMissing)
            return decompressBuffered(input, inputSize, output, PLZ_BUFFER_SIZE); //StatusCode::MissingSizeInfoInHeader;

        size_t outSize = size;
        //uint8_t out[size];
        std::unique_ptr<uint8_t[]> out(new uint8_t[size]);

        size_t inSize = inputSize - propsSize;
        int rc = plz::c::LzmaUncompress(&out[0], &outSize, &input[propsSize], &inSize, &input[0], LZMA_PROPS_SIZE);

        StatusCode status = static_cast<StatusCode>(rc);

        output.insert(output.end(), out.get(), out.get() + outSize);

        return status;
    }

    /*!
     * Prefer using decompress().
     *
     * Only use this if you for some reason required the decompression to be buffered or somehow think you can benefit from reading the data buffered.
     * decompressBuffered() will probably always be slower than the regular decompress() when the LZMA header contains a known file size.
     * decompress() will automatically call decompressBuffered() when file size in LZMA header is unknown.
     *
     * When file size is unknown in the LZMA header, data is always read using the buffered system.
     * This will be slightly slower than normal decompression, but does not require the file result to be known
     *
     * @param input The input compressed data
     * @param output The decompressed data
     * @param bufferSize The buffer size. Default buffer size for pocketlzma is 65536 bytes.
     * @return Status for the decompression process
     */
    StatusCode PocketLzma::decompressBuffered(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint32_t bufferSize)
    {
        return decompressBuffered(&input[0], input.size(), output, bufferSize);
    }

    /*!
     * Prefer using decompress().
     *
     * Only use this if you for some reason required the decompression to be buffered or somehow think you can benefit from reading the data buffered.
     * decompressBuffered() will probably always be slower than the regular decompress() when the LZMA header contains a known file size.
     * decompress() will automatically call decompressBuffered() when file size in LZMA header is unknown.
     *
     * When file size is unknown in the LZMA header, data is always read using the buffered system.
     * This will be slightly slower than normal decompression, but does not require the file result to be known
     *
     * @param input The input data
     * @param output The decompressed data
     * @param bufferSize The buffer size. Default buffer size for pocketlzma is 65536 bytes.
     * @return Status for the decompression process
     */
    StatusCode PocketLzma::decompressBuffered(const uint8_t *input, const size_t inputSize, std::vector<uint8_t> &output, uint32_t bufferSize)
    {
        if(inputSize <= PLZ_MINIMUM_LZMA_SIZE)
            return StatusCode::InvalidLzmaData;

        //size_t unpackSize = 0;

        plz::c::CLzmaDec state;
        size_t propsSize = LZMA_PROPS_SIZE + 8; //header + decompress_size

        /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
        unsigned char header[LZMA_PROPS_SIZE + 8]; //MSVC requires this fully constant...

        //Read header data
        for(int i = 0; i < propsSize; ++i)
            header[i] = input[i];

        LzmaDec_Construct(&state);
        int res = 0;
        res = LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &plz::c::g_Alloc);

        //uint8_t outBuf[bufferSize];
        std::unique_ptr<uint8_t[]> outBuf(new uint8_t[bufferSize]);
        size_t inPos = 0, inSize = 0, outPos = 0;
        inSize = inputSize - propsSize;
        plz::c::LzmaDec_Init(&state);
        for (;;)
        {
            {
                plz::c::SizeT inProcessed = inSize - inPos;
                plz::c::SizeT outProcessed = bufferSize - outPos;
                plz::c::ELzmaFinishMode finishMode = plz::c::LZMA_FINISH_ANY;
                plz::c::ELzmaStatus status;


                res = plz::c::LzmaDec_DecodeToBuf(&state, outBuf.get() + outPos, &outProcessed,
                                                  &input[propsSize] + inPos, &inProcessed, finishMode, &status);


                inPos += inProcessed;
                outPos += outProcessed;
                //unpackSize -= outProcessed;

                output.insert(output.end(), outBuf.get(), outBuf.get() + outPos);

                outPos = 0;

                if (res != SZ_OK)
                    break;

                if (inProcessed == 0 && outProcessed == 0)
                {
                    if (status != plz::c::LZMA_STATUS_FINISHED_WITH_MARK)
                    {
                        LzmaDec_Free(&state, &plz::c::g_Alloc);
                        return static_cast<StatusCode>(SZ_ERROR_DATA);
                    }
                    break;
                }
            }
        }
        LzmaDec_Free(&state, &plz::c::g_Alloc);

        return static_cast<StatusCode>(res);
    }

}

#endif //POCKETLZMA_POCKETLZMA_CLASS_HPP
