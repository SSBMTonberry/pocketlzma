#define CATCH_CONFIG_MAIN
#include "../external_libs/catch.hpp"

//#define POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER

#ifdef POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
    #include "../pocketlzma.hpp"
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

    plz::StatusCode code;
    plz::PocketLzma lzma;


}

TEST_CASE( "Compress a ridiculously small byte buffer - pray that it works", "[compression]" )
{

    plz::PocketLzma p;

    std::vector<uint8_t> input {34, 32, 156, 4, 4, 4, 9};
    std::vector<uint8_t> output;
    plz::StatusCode status = p.compress(input, output);

    REQUIRE(status == plz::StatusCode::Ok);
}

TEST_CASE( "Compress json with default settings - expect smaller compressed size then same data when uncompressed", "[compression]" )
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::string pathOut = "./../../content/to_compress/to/json_test.lzma";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    plz::StatusCode status = p.compress(input, output);

    REQUIRE(status == plz::StatusCode::Ok);
    REQUIRE(output.size() < input.size());

    plz::File::ToFile(pathOut, output);

    std::vector<uint8_t> decompressOutput;
    plz::StatusCode decompStats = p.decompress(output, decompressOutput);

    REQUIRE(decompStats == plz::StatusCode::Ok);
    REQUIRE(input.size() == decompressOutput.size());

}

TEST_CASE( "Decompress lzma-json  - expect larger size and success", "[compression]" )
{
    //std::string path = "./../../content/to_decompress/from/json_test.json.lzma";
    std::string path = "./../../content/to_decompress/from/json_test2.lzma";
    std::string pathOut = "./../../content/to_decompress/to/json_test.json";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    plz::StatusCode status = p.decompress(input, output);

    REQUIRE(status == plz::StatusCode::Ok);
    REQUIRE(output.size() > input.size());

    plz::File::ToFile(pathOut, output);
}

TEST_CASE( "Decompress lzma-json with missing size header - expect missing header error", "[compression]" )
{
    std::string path = "./../../content/to_decompress/from/json_test.json.lzma";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    plz::StatusCode status = p.decompress(input, output);

    REQUIRE(status == plz::StatusCode::MissingSizeInfoInHeader);
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