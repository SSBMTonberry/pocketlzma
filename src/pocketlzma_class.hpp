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
            explicit PocketLzma(const Settings &settings) : m_settings {settings} {};
            inline void setSettings(const Settings &settings);

            inline StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output);
            inline StatusCode decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output);

        private:
            Settings m_settings {};
    };
    /*!
     * This is optional. PocketLzma uses default values if not set by the user.
     * @param settings new settings
     */
    void PocketLzma::setSettings(const Settings &settings)
    {
        m_settings = settings;
    }

    StatusCode PocketLzma::compress(const std::vector <uint8_t> &input, std::vector <uint8_t> &output)
    {
        m_settings.validate();
        size_t propsSize = LZMA_PROPS_SIZE;
        //uint8_t propsEncoded[propsSize + 8];
        uint8_t propsEncoded[propsSize];

        size_t outSize = input.size() + (input.size() / 3) + 128;
        uint8_t out[outSize];

        //int rc = plz::c::LzmaCompress(out, &outSize, &input[0], input.size(), propsEncoded, &propsSize, m_settings.level, m_settings.dictionarySize,
        //                     m_settings.literalContextBits,m_settings.literalPositionBits,m_settings.positionBits,m_settings.fastBytes,1);
        //uint8_t *out;
        //size_t outSize;
        int rc = plz::c::LzmaCompress(out, &outSize, &input[0], input.size(), propsEncoded, &propsSize, m_settings.level, m_settings.dictionarySize,
                             m_settings.literalContextBits,m_settings.literalPositionBits,m_settings.positionBits,m_settings.fastBytes,1);

        //std::vector<uint8_t> buffer(out, out + outSize);
        //output.insert(output.end(), buffer.begin(), buffer.end());

        //output.resize(outSize);
        StatusCode status = static_cast<StatusCode>(rc);
        if(status == StatusCode::Ok)
        {
            std::vector<uint8_t> sizeBits;
            for (int i = 0; i < 8; i++)
                sizeBits.push_back((input.size() >> (i * 8)) & 0xFF);

            output.insert(output.end(), propsEncoded, propsEncoded + propsSize);
            output.insert(output.end(), sizeBits.begin(), sizeBits.end());
            output.insert(output.end(), out, out + outSize);
        }

        //delete[] out; //RBP: Make sure memory is freed
        //free(out);

        return status;
    }

    StatusCode PocketLzma::decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
    {
        size_t propsSize = LZMA_PROPS_SIZE + 8; //header + decompress size

        //size_t inSize = input.size();
        //uint8_t out[1 << 16];
        //size_t outSize = 1 << 16;
        //output.resize(5000000);


        //Extract size

        size_t size = 0;//5000000;
        for (int i = 0; i < 8; i++)
            size |= (input[5 + i] << (i * 8));

        //uint8_t *out;
        //std::unique_ptr<uint8_t> out = std::unique_ptr<uint8_t>();
        std::unique_ptr<uint8_t[]> out = std::unique_ptr<uint8_t[]>(new uint8_t[size]);
        //size_t outSize;
        size_t outSize = size;

        size_t inSize = input.size() - propsSize;
        //int rc = plz::c::LzmaUncompress(out, &outSize, &input[0], &inSize, propsEncoded, propsSize);
        int rc = plz::c::LzmaUncompress(out.get(), &outSize, &input[propsSize], &inSize, &input[0], LZMA_PROPS_SIZE);
        //int rc = plz::c::LzmaUncompress(out, &outSize, &input[propsSize], &inSize, &input[0], LZMA_PROPS_SIZE);

        StatusCode status = static_cast<StatusCode>(rc);

        output.insert(output.end(), out.get(), out.get() + outSize);
        //output.insert(output.end(), out, out + outSize);
        //free(out); //RBP: Make sure memory is freed


        return status;
    }

}

#endif //POCKETLZMA_POCKETLZMA_CLASS_HPP
