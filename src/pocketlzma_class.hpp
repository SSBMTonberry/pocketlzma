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
            inline StatusCode decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output);

        private:
            inline StatusCode decompressBuffered(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint32_t bufferSize = PLZ_BUFFER_SIZE);
            Settings m_settings {};
    };

    PocketLzma::PocketLzma(Preset preset)
    {
        usePreset(preset);
    }

    /*!
     * This is optional. PocketLzma uses default values if not set by the user.
     * @param settings new settings
     */
    void PocketLzma::setSettings(const Settings &settings)
    {
        m_settings = settings;
    }

    void PocketLzma::usePreset(Preset preset)
    {
        m_settings.usePreset(preset);
    }

    StatusCode PocketLzma::compress(const std::vector <uint8_t> &input, std::vector <uint8_t> &output)
    {
        m_settings.validate();
        size_t propsSize = LZMA_PROPS_SIZE;
        uint8_t propsEncoded[propsSize];

        size_t outSize = input.size() + (input.size() / 3) + 128;
        uint8_t out[outSize];

        int rc = plz::c::LzmaCompress(out, &outSize, &input[0], input.size(), propsEncoded, &propsSize, m_settings.level, m_settings.dictionarySize,
                             m_settings.literalContextBits,m_settings.literalPositionBits,m_settings.positionBits,m_settings.fastBytes,1);

        StatusCode status = static_cast<StatusCode>(rc);
        if(status == StatusCode::Ok)
        {
            std::vector<uint8_t> sizeBits;
            for (int i = 0; i < 8; i++)
                sizeBits.push_back((input.size() >> (i * 8)) & 0xFF);

            output.insert(output.end(), propsEncoded, propsEncoded + propsSize); // Property header
            output.insert(output.end(), sizeBits.begin(), sizeBits.end());           // Add decompress size information
            output.insert(output.end(), out, out + outSize);                     // Data
        }

        return status;
    }

    StatusCode PocketLzma::decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {
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
            return decompressBuffered(input, output, PLZ_BUFFER_SIZE); //StatusCode::MissingSizeInfoInHeader;

        uint8_t out[size];
        size_t outSize = size;

        size_t inSize = input.size() - propsSize;
        int rc = plz::c::LzmaUncompress(out, &outSize, &input[propsSize], &inSize, &input[0], LZMA_PROPS_SIZE);

        StatusCode status = static_cast<StatusCode>(rc);

        output.insert(output.end(), out, out + outSize);

        return status;
    }

    StatusCode PocketLzma::decompressBuffered(const std::vector<uint8_t> &input, std::vector<uint8_t> &output, uint32_t bufferSize)
    {
        size_t unpackSize;

        plz::c::CLzmaDec state;
        size_t propsSize = LZMA_PROPS_SIZE + 8; //header + decompress_size

        /* header: 5 bytes of LZMA properties and 8 bytes of uncompressed size */
        unsigned char header[propsSize];

        //Read header data
        for(int i = 0; i < propsSize; ++i)
            header[i] = input[i];

        LzmaDec_Construct(&state);
        int res = 0;
        res = LzmaDec_Allocate(&state, header, LZMA_PROPS_SIZE, &plz::c::g_Alloc);

        uint8_t outBuf[bufferSize];
        size_t inPos = 0, inSize = 0, outPos = 0;
        inSize = input.size() - propsSize;
        plz::c::LzmaDec_Init(&state);
        for (;;)
        {
            {
                plz::c::SizeT inProcessed = inSize - inPos;
                plz::c::SizeT outProcessed = bufferSize - outPos;
                plz::c::ELzmaFinishMode finishMode = plz::c::LZMA_FINISH_ANY;
                plz::c::ELzmaStatus status;

                res = plz::c::LzmaDec_DecodeToBuf(&state, outBuf + outPos, &outProcessed,
                                          &input[propsSize] + inPos, &inProcessed, finishMode, &status);
                inPos += inProcessed;
                outPos += outProcessed;
                unpackSize -= outProcessed;

                output.insert(output.end(), outBuf, outBuf + outPos);

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
