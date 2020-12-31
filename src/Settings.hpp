//
// Created by robin on 29.12.2020.
//

#ifndef POCKETLZMA_SETTINGS_HPP
#define POCKETLZMA_SETTINGS_HPP

namespace plz
{
    class Settings
    {
        public:

            Settings() = default;
            inline explicit Settings(Preset preset);
            /*!
             * Makes sure no values are out of valid range
             */
            inline void validate();

            inline void usePreset(Preset preset);

            /*!
             * level - compression level: 0 <= level <= 9;

             * level dictSize algo  fb
             *   0:    16 KB   0    32
             *   1:    64 KB   0    32
             *   2:   256 KB   0    32
             *   3:     1 MB   0    32
             *   4:     4 MB   0    32
             *   5:    16 MB   1    32
             *   6:    32 MB   1    32
             *   7+:   64 MB   1    64
             *
             * The default value for "level" is 5.
             *
             * algo = 0 means fast method
             * algo = 1 means normal method
             */
            uint8_t level {5};

            /*!
             * The dictionary size in bytes. The maximum value is
             * 128 MB = (1 << 27) bytes for 32-bit version
             *   1 GB = (1 << 30) bytes for 64-bit version
             * The default value is 16 MB = (1 << 24) bytes.
             * It's recommended to use the dictionary that is larger than 4 KB and
             * that can be calculated as (1 << N) or (3 << N) sizes.
             *
             * pocketlzma has a lower limit of (1 << 8) (256 bytes)
             */
            uint32_t dictionarySize {1 << 24};

            /*!
             * lc - The number of literal context bits (high bits of previous literal).
             * It can be in the range from 0 to 8. The default value is 3.
             * Sometimes lc=4 gives the gain for big files.
             */
            uint8_t literalContextBits {3};

            /*!
             * lp - The number of literal pos bits (low bits of current position for literals).
             * It can be in the range from 0 to 4. The default value is 0.
             * The lp switch is intended for periodical data when the period is equal to 2^lp.
             * For example, for 32-bit (4 bytes) periodical data you can use lp=2. Often it's
             * better to set lc=0, if you change lp switch.
             */
            uint8_t literalPositionBits {0};

            /*!
             * pb - The number of pos bits (low bits of current position).
                    It can be in the range from 0 to 4. The default value is 2.
                    The pb switch is intended for periodical data when the period is equal 2^pb.
             */
            uint8_t positionBits {2};

            /*!
             * fb - Word size (the number of fast bytes).
             *      It can be in the range from 5 to 273. The default value is 32.
             *      Usually, a big number gives a little bit better compression ratio and
             *      slower compression process.
             */
            uint16_t fastBytes {32};
    };

    Settings::Settings(Preset preset)
    {
        usePreset(preset);
    }

    void Settings::validate()
    {
        if(level > PLZ_MAX_LEVEL)
            level = PLZ_MAX_LEVEL;

        if(dictionarySize < PLZ_MIN_DICTIONARY_SIZE)
            dictionarySize = PLZ_MIN_DICTIONARY_SIZE;
        else if(dictionarySize > PLZ_MAX_DICTIONARY_SIZE)
            dictionarySize = PLZ_MAX_DICTIONARY_SIZE;

        if(literalContextBits > PLZ_MAX_LITERAL_CONTEXT_BITS)
            literalContextBits = PLZ_MAX_LITERAL_CONTEXT_BITS;

        if(literalPositionBits > PLZ_MAX_LITERAL_POSITION_BITS)
            literalPositionBits = PLZ_MAX_LITERAL_POSITION_BITS;

        if(positionBits > PLZ_MAX_POSITION_BITS)
            positionBits = PLZ_MAX_POSITION_BITS;

        if(fastBytes < PLZ_MIN_FAST_BYTES)
            fastBytes = PLZ_MIN_FAST_BYTES;
        else if(fastBytes > PLZ_MAX_FAST_BYTES)
            fastBytes = PLZ_MAX_FAST_BYTES;

    }

    void Settings::usePreset(Preset preset)
    {
        switch(preset)
        {
            case Preset::Default:
                level = 5;
                dictionarySize = 1 << 24;
                literalContextBits = 3;
                literalPositionBits = 0;
                positionBits = 2;
                fastBytes = 32;
                break;

            case Preset::Fastest:
                level = 1;
                dictionarySize = 1 << 16;
                literalContextBits = 4;
                literalPositionBits = 0;
                positionBits = 2;
                fastBytes = 8;
                break;

            case Preset::Fast:
                level = 4;
                dictionarySize = 1 << 22;
                literalContextBits = 4;
                literalPositionBits = 0;
                positionBits = 2;
                fastBytes = 16;
                break;

            case Preset::GoodCompression:
                level = 7;
                dictionarySize = 1 << 26;
                literalContextBits = 3;
                literalPositionBits = 0;
                positionBits = 2;
                fastBytes = 64;
                break;

            case Preset::BestCompression:
                level = 9;
                dictionarySize = 1 << 27;
                literalContextBits = 3;
                literalPositionBits = 0;
                positionBits = 2;
                fastBytes = 128;
                break;



        }
    }

}

#endif //POCKETLZMA_SETTINGS_HPP
