//
// Created by robin on 02.01.2021.
//
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#define LZMA_C_DEFINE
#include "../extras/lzma_c.hpp"

TEST_CASE("Simple compile test for LZMA C - expect compile OK and no error", "[compile]")
{
    const std::vector<uint8_t> input
    {
        34, 32, 156, 4, 4, 4, 9, 78, 192, 82, 33, 91, 34, 32,
        156, 4, 4, 4, 9, 78, 192, 82, 33, 91, 34, 32,
        156, 4, 4, 4, 9, 78, 192, 82, 33, 91
    };

    size_t outputSize = 1024;
    uint8_t output[outputSize];
    uint8_t props[12];
    size_t propsSize = 5;

    int level = 3;
    uint32_t dictionarySize = 1 << 12;
    uint8_t lc = 3;
    uint8_t lp = 0;
    uint8_t pb = 2;
    uint8_t fb = 32;
    int rc = LzmaCompress(output, &outputSize, &input[0], input.size(), props, &propsSize, level, dictionarySize, lc, lp, pb, fb, 1);
    REQUIRE(rc == 0);
}