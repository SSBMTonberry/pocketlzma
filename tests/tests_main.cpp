//
// Created by robin on 31.07.2019.
//

#define CATCH_CONFIG_MAIN
#include "../external_libs/catch.hpp"

#define POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
//#define DISABLE_CPP17_FILESYSTEM

#ifdef POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
    #include "../single_include/pocketlzma.hpp"
#else
    #include "../src/pocketlzma.h"
#endif

#include <memory>
#include <map>
#include <functional>

TEST_CASE("Dummy", "[dumdum]")
{
    unsigned char * dest;
    size_t destLen = 0;
    unsigned char * src;
    size_t srcLen = 0;
    unsigned char * props;
    size_t propsSize = 0;
    plz::c::LzmaCompress(dest, &destLen, src, srcLen, props, &propsSize, 5, (1 << 7), 5,5,4,3,2);
}

//TEST_CASE( "Basic test", "[basic]" )
//{
//    std::unique_ptr<plz::ICompressor> compressor {new plz::Lzma2Compressor()}; //The nasty C++11 way
//    std::vector<uint8_t> input {34, 32, 156, 4, 4, 4, 9};
//    std::vector<uint8_t> output;
//    plz::StatusCode status = compressor->compress(input, output);
//
//    REQUIRE(status == plz::StatusCode::Ok);
//}
//
//TEST_CASE( "Compress json - expect smaller compressed size", "[compression]" )
//{
//    std::string path = "./../../content/to_compress/from/json_test.json";
//    std::vector<uint8_t> input = plz::File::FromFile(path);
//
//    std::unique_ptr<plz::ICompressor> compressor {new plz::Lzma2Compressor()}; //The nasty C++11 way
//    std::vector<uint8_t> output;
//    plz::StatusCode status = compressor->compress(input, output);
//
//    REQUIRE(status == plz::StatusCode::Ok);
//    REQUIRE(output.size() < input.size());
//}