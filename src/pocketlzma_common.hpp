//
// Created by robin on 29.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_COMMON_HPP
#define POCKETLZMA_POCKETLZMA_COMMON_HPP
#include <vector>
#include <cstdint>
#include <memory>

namespace plz
{
    const uint8_t PLZ_MAX_LEVEL {9};
    const uint32_t PLZ_MIN_DICTIONARY_SIZE {1 << 8};
    const uint32_t PLZ_MAX_DICTIONARY_SIZE {1 << 30};
    const uint8_t PLZ_MAX_LITERAL_CONTEXT_BITS {8};
    const uint8_t PLZ_MAX_LITERAL_POSITION_BITS {4};
    const uint8_t PLZ_MAX_POSITION_BITS {4};
    const uint16_t PLZ_MIN_FAST_BYTES {5};
    const uint16_t PLZ_MAX_FAST_BYTES {273};

    enum class StatusCode
    {

        Ok = SZ_OK, //0
        ErrorData = SZ_ERROR_DATA, //1
        ErrorMem = SZ_ERROR_MEM, //2
        ErrorCrc = SZ_ERROR_CRC, //3
        ErrorUnsupported = SZ_ERROR_UNSUPPORTED, //4
        ErrorParam = SZ_ERROR_PARAM, //5
        ErrorInputEof = SZ_ERROR_INPUT_EOF, //6
        ErrorOutputEof = SZ_ERROR_OUTPUT_EOF, //7
        ErrorRead = SZ_ERROR_READ, //8
        ErrorWrite = SZ_ERROR_WRITE, //9
        ErrorProgress = SZ_ERROR_PROGRESS, //10
        ErrorFail = SZ_ERROR_FAIL, //11
        ErrorThread = SZ_ERROR_THREAD, //12
        ErrorArchive = SZ_ERROR_ARCHIVE, //16
        ErrorNoArchive = SZ_ERROR_NO_ARCHIVE, //17

        UndefinedError = 999
    };
}
#endif //POCKETLZMA_POCKETLZMA_COMMON_HPP
