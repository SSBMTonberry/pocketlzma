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
        size_t outSize = input.size();
        uint8_t out[outSize];

        int rc = plz::c::LzmaCompress(out, &outSize, &input[0], input.size(), propsEncoded, &propsSize, m_settings.level, m_settings.dictionarySize,
                             m_settings.literalContextBits,m_settings.literalPositionBits,m_settings.positionBits,m_settings.fastBytes,1);

        std::vector<uint8_t> buffer(out, out + outSize);
        output.insert(output.end(), buffer.begin(), buffer.end());

        StatusCode status = static_cast<StatusCode>(rc);

        return status;
    }

}

#endif //POCKETLZMA_POCKETLZMA_CLASS_HPP
