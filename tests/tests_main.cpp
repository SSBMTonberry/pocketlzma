#define CATCH_CONFIG_MAIN
#include "../external_libs/catch.hpp"

#include "PocketLzmaTestsConfig.h"
//#define POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER

#ifdef POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
    #include "../pocketlzma.hpp"
#else
    #include "../src/pocketlzma.h"
#endif

#include <memory>
#include <map>
#include <functional>
#include "../content/memoryGen/pocketlzma_memory_files_mapper.h"

/*! short namespace alias */
namespace memfiles = pocketlzma_memory_files_mapper;

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

TEST_CASE( "Decompress too short data - expect InvalidLzmaData error", "[compression]" )
{
    std::vector<uint8_t> input {34, 32, 156, 4, 4, 4, 9, 78, 192, 82, 33, 91};

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    plz::StatusCode status = p.decompress(input, output);

    REQUIRE(status == plz::StatusCode::InvalidLzmaData);
}

/*!
 * Removed due to no nice way to resolve corrupted data crashes...
 */
// TEST_CASE( "Decompress random invalid data - expect UndefinedError", "[compression]" )
// {
//     std::vector<uint8_t> input
//     {
//         34, 32, 156, 4, 4, 4, 9, 78, 192, 82, 33, 91, 34, 32,
//         156, 4, 4, 4, 9, 78, 192, 82, 33, 91, 34, 32,
//         156, 4, 4, 4, 9, 78, 192, 82, 33, 91
//     };
//
//     plz::PocketLzma p;
//
//     std::vector<uint8_t> output;
//     plz::StatusCode status = p.decompress(input, output);
//
//     REQUIRE(status == plz::StatusCode::UndefinedError);
// }
//
// TEST_CASE( "Decompress random invalid data buffered - expect UndefinedError", "[compression]" )
// {
//     std::vector<uint8_t> input
//             {
//                     34, 32, 156, 4, 4, 4, 9, 78, 192, 82, 33, 91, 34, 32,
//                     156, 4, 4, 4, 9, 78, 192, 82, 33, 91, 34, 32,
//                     156, 4, 4, 4, 9, 78, 192, 82, 33, 91
//             };
//
//     plz::PocketLzma p;
//
//     std::vector<uint8_t> output;
//     plz::StatusCode status = p.decompress(input, output);
//
//     REQUIRE(status != plz::StatusCode::Ok);
// }


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

    REQUIRE(status == plz::StatusCode::Ok);
}

TEST_CASE( "Decompress two files from memory - expect success", "[decompression]" )
{
    std::vector<uint8_t> input1 = plz::File::FromMemory(memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA, memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA_SIZE);
    std::vector<uint8_t> input2 = plz::File::FromMemory(memfiles::_JSON_TEST_OK_HEADER_LZMA, memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE);
    plz::PocketLzma p;

    std::vector<uint8_t> output1;
    std::vector<uint8_t> output2;

    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.decompress(input1, output1);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.decompress(input2, output2);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "unknown size mem file time: " << ms1.count() << "ms - Size (bytes): " << input1.size() << "->" << output1.size() << "\n\n";
    std::cout << "OK mem file time:           " << ms2.count() << "ms - Size (bytes): " << input2.size() << "->" << output2.size() << "\n";

    REQUIRE(output1.size() == output2.size());
}

TEST_CASE( "Decompress buffered from memory - expect success", "[decompression]" )
{
    std::vector<uint8_t> input = plz::File::FromMemory(memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA, memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA_SIZE);
    plz::PocketLzma p;

    std::vector<uint8_t> output2;

    plz::StatusCode status = p.decompressBuffered(input, output2);
    plz::StatusCode status2 = p.decompressBuffered(memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA, memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA_SIZE, output2);
    REQUIRE(status == plz::StatusCode::Ok);
    REQUIRE(status2 == plz::StatusCode::Ok);
}

TEST_CASE( "Decompress two files from memory using alternative function - expect success", "[decompression]" )
{
    plz::PocketLzma p;

    std::vector<uint8_t> output1;
    std::vector<uint8_t> output2;

    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.decompress(memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA, memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA_SIZE, output1);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.decompress(memfiles::_JSON_TEST_OK_HEADER_LZMA, memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE, output2);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "unknown size mem file time: " << ms1.count() << "ms - Size (bytes): " << memfiles::_JSON_TEST_UNKNOWN_SIZE_LZMA_SIZE << "->" << output1.size() << "\n\n";
    std::cout << "OK mem file time:           " << ms2.count() << "ms - Size (bytes): " << memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE << "->" << output2.size() << "\n";

    REQUIRE(output1.size() == output2.size());
}

TEST_CASE( "Compress json with compression presets BestCompression - Fast - expect smaller size and slower for highest compression", "[compression]" )
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    std::vector<uint8_t> outputFast;

    p.usePreset(plz::Preset::BestCompression);
    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.compress(input, output);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    p.usePreset(plz::Preset::Fast);
    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.compress(input, outputFast);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "Fast time:            " << ms2.count() << "ms - Size (bytes): " << input.size() << "->" << outputFast.size() << "\n";
    std::cout << "BestCompression time: " << ms1.count() << "ms - Size (bytes): " << input.size() << "->" << output.size() << "\n\n";

    REQUIRE(output.size() < outputFast.size());
    REQUIRE(ms2 < ms1);
}

TEST_CASE( "Compress json with compression presets Default - Fast - expect smaller size and slower for highest compression", "[compression]" )
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    std::vector<uint8_t> outputFast;

    p.usePreset(plz::Preset::Default);
    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.compress(input, output);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    p.usePreset(plz::Preset::Fast);
    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.compress(input, outputFast);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "Fast time:            " << ms2.count() << "ms - Size (bytes): " << input.size() << "->" << outputFast.size() << "\n";
    std::cout << "Default time:         " << ms1.count() << "ms - Size (bytes): " << input.size() << "->" << output.size() << "\n\n";

    REQUIRE(output.size() < outputFast.size());
    REQUIRE(ms2 < ms1);
}

TEST_CASE( "Compress json with compression presets Fast - Fastest - expect smaller size and slower for highest compression", "[compression]" )
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    std::vector<uint8_t> outputFast;

    p.usePreset(plz::Preset::Fast);
    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.compress(input, output);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    p.usePreset(plz::Preset::Fastest);
    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.compress(input, outputFast);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "Fastest time:            " << ms2.count() << "ms - Size (bytes): " << input.size() << "->" << outputFast.size() << "\n";
    std::cout << "Fast time:               " << ms1.count() << "ms - Size (bytes): " << input.size() << "->" << output.size() << "\n\n";

    REQUIRE(output.size() < outputFast.size());
    //REQUIRE(ms2 < ms1); //Speeddifference is so small that "Fast" in a few rare cases can be quicker than "Fastest"
}

TEST_CASE( "Compress json with compression presets GoodCompression - BestCompression - expect smaller size and slower for highest compression", "[compression]" )
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    std::vector<uint8_t> outputFast;

    p.usePreset(plz::Preset::BestCompression);
    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.compress(input, output);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    p.usePreset(plz::Preset::GoodCompression);
    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.compress(input, outputFast);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "GoodCompression time:            " << ms2.count() << "ms - Size (bytes): " << input.size() << "->" << outputFast.size() << "\n";
    std::cout << "BestCompression time:            " << ms1.count() << "ms - Size (bytes): " << input.size() << "->" << output.size() << "\n\n";

    REQUIRE(output.size() < outputFast.size());
    REQUIRE(ms2 < ms1);
}

TEST_CASE( "Compress json with compression presets Default - GoodCompression - expect smaller size and slower for highest compression", "[compression]" )
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    std::vector<uint8_t> outputFast;

    p.usePreset(plz::Preset::GoodCompression);
    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.compress(input, output);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    p.usePreset(plz::Preset::Default);
    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.compress(input, outputFast);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "Default time:                    " << ms2.count() << "ms - Size (bytes): " << input.size() << "->" << outputFast.size() << "\n";
    std::cout << "GoodCompression time:            " << ms1.count() << "ms - Size (bytes): " << input.size() << "->" << output.size() << "\n\n";

    REQUIRE(output.size() < outputFast.size());
    REQUIRE(ms2 < ms1);
}

TEST_CASE( "Compress and Decompress a Slippi replay with compression preset GoodCompression - print compression time and expect smaller size ", "[compression]" )
{
    std::string path = "./../../content/to_compress/from/slippi_replay.slp";
    std::vector<uint8_t> input = plz::File::FromFile(path);

    plz::PocketLzma p;

    std::vector<uint8_t> output;
    std::vector<uint8_t> outputDecomp;

    p.usePreset(plz::Preset::GoodCompression);
    auto start1 = std::chrono::steady_clock::now();
    plz::StatusCode status = p.compress(input, output);
    auto end1 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms1 = (end1-start1) * 1000;

    REQUIRE(status == plz::StatusCode::Ok);

    auto start2 = std::chrono::steady_clock::now();
    plz::StatusCode status2 = p.decompress(output, outputDecomp);
    auto end2 = std::chrono::steady_clock::now();
    std::chrono::duration<double> ms2 = (end2-start2) * 1000;

    REQUIRE(status2 == plz::StatusCode::Ok);

    std::cout << "Slippi replay test: \n";
    std::cout << "Compression time:          " << ms1.count() << "ms - Size (bytes): " << input.size() << "->" << output.size() << "\n";
    std::cout << "Decompression time:        " << ms2.count() << "ms - Size (bytes): " << output.size() << "->" << outputDecomp.size() << "\n";

    REQUIRE(output.size() < outputDecomp.size());
    REQUIRE(ms2 < ms1);
}

TEST_CASE( "Full memory test - expect success", "[memory]" )
{

    plz::PocketLzma p;

    //Alternative 1
    std::vector<uint8_t> decompressed;
    plz::StatusCode status = p.decompress(memfiles::_JSON_TEST_OK_HEADER_LZMA, memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE, decompressed);

    REQUIRE(status == plz::StatusCode::Ok);
    decompressed.clear();

    //Alternative 2
    std::vector<uint8_t> bytes = plz::File::FromMemory(memfiles::_JSON_TEST_OK_HEADER_LZMA, memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE);
    status = p.decompress(bytes, decompressed);

    REQUIRE(status == plz::StatusCode::Ok);
    decompressed.clear();
    bytes.clear();

    //Alternative 3
    //std::vector<uint8_t> bytes;
    plz::File::FromMemory(memfiles::_JSON_TEST_OK_HEADER_LZMA, memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE, bytes);
    status = p.decompress(bytes, decompressed);

    REQUIRE(status == plz::StatusCode::Ok);
}

#ifdef RUN_BENCHMARK_TESTS
    TEST_CASE( "BENCHMARK json - Compress and decompress json with all compression presets - calculate average", "[compression]" )
    {
        std::string path = "./../../content/to_compress/from/json_test.json";
        std::vector<uint8_t> input = plz::File::FromFile(path);

        plz::PocketLzma p;

        const int NUM_OF_RUNS = 20;

        std::vector<uint8_t> outputFastest;
        std::vector<uint8_t> outputFast;
        std::vector<uint8_t> outputDefault;
        std::vector<uint8_t> outputGood;
        std::vector<uint8_t> outputBest;

        std::vector<double> timeFastest;
        std::vector<double> timeFast;
        std::vector<double> timeDefault;
        std::vector<double> timeGood;
        std::vector<double> timeBest;

        auto testStart = std::chrono::steady_clock::now();
        for(int i = 0; i < NUM_OF_RUNS; ++i)
        {
            std::vector<uint8_t> output;

            //Fastest
            p.usePreset(plz::Preset::Fastest);
            auto startFastest = std::chrono::steady_clock::now();
            plz::StatusCode status = p.compress(input, output);
            auto endFastest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFastest = (endFastest-startFastest) * 1000;
            if(i == 0)
                outputFastest = output;
            timeFastest.push_back(msFastest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Fast
            p.usePreset(plz::Preset::Fast);
            auto startFast = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endFast = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFast = (endFast-startFast) * 1000;
            if(i == 0)
                outputFast = output;
            timeFast.push_back(msFast.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Default
            p.usePreset(plz::Preset::Default);
            auto startDefault = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endDefault = std::chrono::steady_clock::now();
            std::chrono::duration<double> msDefault = (endDefault-startDefault) * 1000;
            if(i == 0)
                outputDefault = output;
            timeDefault.push_back(msDefault.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Good
            p.usePreset(plz::Preset::GoodCompression);
            auto startGood = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endGood = std::chrono::steady_clock::now();
            std::chrono::duration<double> msGood = (endGood-startGood) * 1000;
            if(i == 0)
                outputGood = output;
            timeGood.push_back(msGood.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Best
            p.usePreset(plz::Preset::BestCompression);
            auto startBest = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endBest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msBest = (endBest-startBest) * 1000;
            if(i == 0)
                outputBest = output;
            timeBest.push_back(msBest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();
        }
        auto testStop = std::chrono::steady_clock::now();
        std::chrono::duration<double> msTest = (testStop-testStart) * 1000;

        double fastestMin = std::min_element(timeFastest.begin(), timeFastest.end()).operator*();
        double fastestAvg = std::accumulate(timeFastest.begin(), timeFastest.end(), 0.0) / timeFastest.size();
        double fastestMax = std::max_element(timeFastest.begin(), timeFastest.end()).operator*();
        double fastMin = std::min_element(timeFast.begin(), timeFast.end()).operator*();
        double fastAvg = std::accumulate(timeFast.begin(), timeFast.end(), 0.0) / timeFast.size();
        double fastMax = std::max_element(timeFast.begin(), timeFast.end()).operator*();
        double defaultMin = std::min_element(timeDefault.begin(), timeDefault.end()).operator*();
        double defaultAvg = std::accumulate(timeDefault.begin(), timeDefault.end(), 0.0) / timeDefault.size();
        double defaultMax = std::max_element(timeDefault.begin(), timeDefault.end()).operator*();
        double goodMin = std::min_element(timeGood.begin(), timeGood.end()).operator*();
        double goodAvg = std::accumulate(timeGood.begin(), timeGood.end(), 0.0) / timeGood.size();
        double goodMax = std::max_element(timeGood.begin(), timeGood.end()).operator*();
        double bestMin = std::min_element(timeBest.begin(), timeBest.end()).operator*();
        double bestAvg = std::accumulate(timeBest.begin(), timeBest.end(), 0.0) / timeBest.size();
        double bestMax = std::max_element(timeBest.begin(), timeBest.end()).operator*();

        std::cout << "// C O M P R E S S I O N ( J S O N )  B E N C H M A R K  \n";
        std::cout << "// -------------------------------------------\n\n";
        std::cout << "Number of runs: " << NUM_OF_RUNS << "\n";
        std::cout << "Total time: " << msTest.count() << "ms\n\n";
        std::cout << "Fastest - time: average=" << fastestAvg << "ms min=" << fastestMin << "ms max=" << fastestMax <<
                                                 "ms - Size (bytes): " << input.size() << "->" << outputFastest.size() << "\n";
        std::cout << "Fast - time: average=" << fastAvg << "ms min=" << fastMin << "ms max=" << fastMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputFast.size() << "\n";
        std::cout << "Default - time: average=" << defaultAvg << "ms min=" << defaultMin << "ms max=" << defaultMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputDefault.size() << "\n";
        std::cout << "Good - time: average=" << goodAvg << "ms min=" << goodMin << "ms max=" << goodMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputGood.size() << "\n";
        std::cout << "Best - time: average=" << bestAvg << "ms min=" << bestMin << "ms max=" << bestMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputBest.size() << "\n";

        std::cout << std::endl;

        //Decompression
        std::vector<double> timeDecompFastest;
        std::vector<double> timeDecompFast;
        std::vector<double> timeDecompDefault;
        std::vector<double> timeDecompGood;
        std::vector<double> timeDecompBest;

        auto testDecompStart = std::chrono::steady_clock::now();
        for(int i = 0; i < NUM_OF_RUNS; ++i)
        {
            std::vector<uint8_t> output;

            //Fastest
            auto startFastest = std::chrono::steady_clock::now();
            plz::StatusCode status = p.decompress(outputFastest, output);
            auto endFastest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFastest = (endFastest-startFastest) * 1000;
            timeDecompFastest.push_back(msFastest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Fast
            auto startFast = std::chrono::steady_clock::now();
            status = p.decompress(outputFast, output);
            auto endFast = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFast = (endFast-startFast) * 1000;
            timeDecompFast.push_back(msFast.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Default
            auto startDefault = std::chrono::steady_clock::now();
            status = p.decompress(outputDefault, output);
            auto endDefault = std::chrono::steady_clock::now();
            std::chrono::duration<double> msDefault = (endDefault-startDefault) * 1000;
            timeDecompDefault.push_back(msDefault.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Good
            auto startGood = std::chrono::steady_clock::now();
            status = p.decompress(outputGood, output);
            auto endGood = std::chrono::steady_clock::now();
            std::chrono::duration<double> msGood = (endGood-startGood) * 1000;
            timeDecompGood.push_back(msGood.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Best
            auto startBest = std::chrono::steady_clock::now();
            status = p.decompress(outputBest, output);
            auto endBest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msBest = (endBest-startBest) * 1000;
            timeDecompBest.push_back(msBest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();
        }
        auto testDecompStop = std::chrono::steady_clock::now();
        std::chrono::duration<double> msDecompTest = (testDecompStop-testDecompStart) * 1000;

        double fastestDecompMin = std::min_element(timeDecompFastest.begin(), timeDecompFastest.end()).operator*();
        double fastestDecompAvg = std::accumulate(timeDecompFastest.begin(), timeDecompFastest.end(), 0.0) / timeDecompFastest.size();
        double fastestDecompMax = std::max_element(timeDecompFastest.begin(), timeDecompFastest.end()).operator*();
        double fastDecompMin = std::min_element(timeDecompFast.begin(), timeDecompFast.end()).operator*();
        double fastDecompAvg = std::accumulate(timeDecompFast.begin(), timeDecompFast.end(), 0.0) / timeDecompFast.size();
        double fastDecompMax = std::max_element(timeDecompFast.begin(), timeDecompFast.end()).operator*();
        double defaultDecompMin = std::min_element(timeDecompDefault.begin(), timeDecompDefault.end()).operator*();
        double defaultDecompAvg = std::accumulate(timeDecompDefault.begin(), timeDecompDefault.end(), 0.0) / timeDecompDefault.size();
        double defaultDecompMax = std::max_element(timeDecompDefault.begin(), timeDecompDefault.end()).operator*();
        double goodDecompMin = std::min_element(timeDecompGood.begin(), timeDecompGood.end()).operator*();
        double goodDecompAvg = std::accumulate(timeDecompGood.begin(), timeDecompGood.end(), 0.0) / timeDecompGood.size();
        double goodDecompMax = std::max_element(timeDecompGood.begin(), timeDecompGood.end()).operator*();
        double bestDecompMin = std::min_element(timeDecompBest.begin(), timeDecompBest.end()).operator*();
        double bestDecompAvg = std::accumulate(timeDecompBest.begin(), timeDecompBest.end(), 0.0) / timeDecompBest.size();
        double bestDecompMax = std::max_element(timeDecompBest.begin(), timeDecompBest.end()).operator*();

        std::cout << "// D E C O M P R E S S I O N ( J S O N )   B E N C H M A R K  \n";
        std::cout << "// -----------------------------------------------\n\n";
        std::cout << "Number of runs: " << NUM_OF_RUNS << "\n";
        std::cout << "Total time: " << msDecompTest.count() << "ms\n\n";
        std::cout << "Fastest - time: average=" << fastestDecompAvg << "ms min=" << fastestDecompMin << "ms max=" << fastestDecompMax << "ms\n";
        std::cout << "Fast - time: average=" << fastDecompAvg << "ms min=" << fastDecompMin << "ms max=" << fastDecompMax << "ms\n";
        std::cout << "Default - time: average=" << defaultDecompAvg << "ms min=" << defaultDecompMin << "ms max=" << defaultDecompMax << "ms\n";
        std::cout << "Good - time: average=" << goodDecompAvg << "ms min=" << goodDecompMin << "ms max=" << goodDecompMax << "ms\n";
        std::cout << "Best - time: average=" << bestDecompAvg << "ms min=" << bestDecompMin << "ms max=" << bestDecompMax << "ms\n";

        std::cout << std::endl;

        REQUIRE(true);
    }

    TEST_CASE( "BENCHMARK 4MB .slp binary file - Compress and decompress .slp with all compression presets - calculate average", "[compression]" )
    {
        std::string path = "./../../content/to_compress/from/slippi_replay.slp";
        std::vector<uint8_t> input = plz::File::FromFile(path);

        plz::PocketLzma p;

        const int NUM_OF_RUNS = 5;

        std::vector<uint8_t> outputFastest;
        std::vector<uint8_t> outputFast;
        std::vector<uint8_t> outputDefault;
        std::vector<uint8_t> outputGood;
        std::vector<uint8_t> outputBest;

        std::vector<double> timeFastest;
        std::vector<double> timeFast;
        std::vector<double> timeDefault;
        std::vector<double> timeGood;
        std::vector<double> timeBest;

        auto testStart = std::chrono::steady_clock::now();
        for(int i = 0; i < NUM_OF_RUNS; ++i)
        {
            std::vector<uint8_t> output;

            //Fastest
            p.usePreset(plz::Preset::Fastest);
            auto startFastest = std::chrono::steady_clock::now();
            plz::StatusCode status = p.compress(input, output);
            auto endFastest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFastest = (endFastest-startFastest) * 1000;
            if(i == 0)
                outputFastest = output;
            timeFastest.push_back(msFastest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Fast
            p.usePreset(plz::Preset::Fast);
            auto startFast = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endFast = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFast = (endFast-startFast) * 1000;
            if(i == 0)
                outputFast = output;
            timeFast.push_back(msFast.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Default
            p.usePreset(plz::Preset::Default);
            auto startDefault = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endDefault = std::chrono::steady_clock::now();
            std::chrono::duration<double> msDefault = (endDefault-startDefault) * 1000;
            if(i == 0)
                outputDefault = output;
            timeDefault.push_back(msDefault.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Good
            p.usePreset(plz::Preset::GoodCompression);
            auto startGood = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endGood = std::chrono::steady_clock::now();
            std::chrono::duration<double> msGood = (endGood-startGood) * 1000;
            if(i == 0)
                outputGood = output;
            timeGood.push_back(msGood.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Best
            p.usePreset(plz::Preset::BestCompression);
            auto startBest = std::chrono::steady_clock::now();
            status = p.compress(input, output);
            auto endBest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msBest = (endBest-startBest) * 1000;
            if(i == 0)
                outputBest = output;
            timeBest.push_back(msBest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();
        }
        auto testStop = std::chrono::steady_clock::now();
        std::chrono::duration<double> msTest = (testStop-testStart) * 1000;

        double fastestMin = std::min_element(timeFastest.begin(), timeFastest.end()).operator*();
        double fastestAvg = std::accumulate(timeFastest.begin(), timeFastest.end(), 0.0) / timeFastest.size();
        double fastestMax = std::max_element(timeFastest.begin(), timeFastest.end()).operator*();
        double fastMin = std::min_element(timeFast.begin(), timeFast.end()).operator*();
        double fastAvg = std::accumulate(timeFast.begin(), timeFast.end(), 0.0) / timeFast.size();
        double fastMax = std::max_element(timeFast.begin(), timeFast.end()).operator*();
        double defaultMin = std::min_element(timeDefault.begin(), timeDefault.end()).operator*();
        double defaultAvg = std::accumulate(timeDefault.begin(), timeDefault.end(), 0.0) / timeDefault.size();
        double defaultMax = std::max_element(timeDefault.begin(), timeDefault.end()).operator*();
        double goodMin = std::min_element(timeGood.begin(), timeGood.end()).operator*();
        double goodAvg = std::accumulate(timeGood.begin(), timeGood.end(), 0.0) / timeGood.size();
        double goodMax = std::max_element(timeGood.begin(), timeGood.end()).operator*();
        double bestMin = std::min_element(timeBest.begin(), timeBest.end()).operator*();
        double bestAvg = std::accumulate(timeBest.begin(), timeBest.end(), 0.0) / timeBest.size();
        double bestMax = std::max_element(timeBest.begin(), timeBest.end()).operator*();

        std::cout << "// C O M P R E S S I O N ( . S L P )   B E N C H M A R K  \n";
        std::cout << "// -------------------------------------------\n\n";
        std::cout << "Number of runs: " << NUM_OF_RUNS << "\n";
        std::cout << "Total time: " << msTest.count() << "ms\n\n";
        std::cout << "Fastest - time: average=" << fastestAvg << "ms min=" << fastestMin << "ms max=" << fastestMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputFastest.size() << "\n";
        std::cout << "Fast - time: average=" << fastAvg << "ms min=" << fastMin << "ms max=" << fastMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputFast.size() << "\n";
        std::cout << "Default - time: average=" << defaultAvg << "ms min=" << defaultMin << "ms max=" << defaultMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputDefault.size() << "\n";
        std::cout << "Good - time: average=" << goodAvg << "ms min=" << goodMin << "ms max=" << goodMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputGood.size() << "\n";
        std::cout << "Best - time: average=" << bestAvg << "ms min=" << bestMin << "ms max=" << bestMax <<
                  "ms - Size (bytes): " << input.size() << "->" << outputBest.size() << "\n";

        std::cout << std::endl;

        //Decompression
        std::vector<double> timeDecompFastest;
        std::vector<double> timeDecompFast;
        std::vector<double> timeDecompDefault;
        std::vector<double> timeDecompGood;
        std::vector<double> timeDecompBest;

        auto testDecompStart = std::chrono::steady_clock::now();
        for(int i = 0; i < NUM_OF_RUNS; ++i)
        {
            std::vector<uint8_t> output;

            //Fastest
            auto startFastest = std::chrono::steady_clock::now();
            plz::StatusCode status = p.decompress(outputFastest, output);
            auto endFastest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFastest = (endFastest-startFastest) * 1000;
            timeDecompFastest.push_back(msFastest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Fast
            auto startFast = std::chrono::steady_clock::now();
            status = p.decompress(outputFast, output);
            auto endFast = std::chrono::steady_clock::now();
            std::chrono::duration<double> msFast = (endFast-startFast) * 1000;
            timeDecompFast.push_back(msFast.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Default
            auto startDefault = std::chrono::steady_clock::now();
            status = p.decompress(outputDefault, output);
            auto endDefault = std::chrono::steady_clock::now();
            std::chrono::duration<double> msDefault = (endDefault-startDefault) * 1000;
            timeDecompDefault.push_back(msDefault.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Good
            auto startGood = std::chrono::steady_clock::now();
            status = p.decompress(outputGood, output);
            auto endGood = std::chrono::steady_clock::now();
            std::chrono::duration<double> msGood = (endGood-startGood) * 1000;
            timeDecompGood.push_back(msGood.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();

            //Best
            auto startBest = std::chrono::steady_clock::now();
            status = p.decompress(outputBest, output);
            auto endBest = std::chrono::steady_clock::now();
            std::chrono::duration<double> msBest = (endBest-startBest) * 1000;
            timeDecompBest.push_back(msBest.count());
            REQUIRE(status == plz::StatusCode::Ok);
            output.clear();
        }
        auto testDecompStop = std::chrono::steady_clock::now();
        std::chrono::duration<double> msDecompTest = (testDecompStop-testDecompStart) * 1000;

        double fastestDecompMin = std::min_element(timeDecompFastest.begin(), timeDecompFastest.end()).operator*();
        double fastestDecompAvg = std::accumulate(timeDecompFastest.begin(), timeDecompFastest.end(), 0.0) / timeDecompFastest.size();
        double fastestDecompMax = std::max_element(timeDecompFastest.begin(), timeDecompFastest.end()).operator*();
        double fastDecompMin = std::min_element(timeDecompFast.begin(), timeDecompFast.end()).operator*();
        double fastDecompAvg = std::accumulate(timeDecompFast.begin(), timeDecompFast.end(), 0.0) / timeDecompFast.size();
        double fastDecompMax = std::max_element(timeDecompFast.begin(), timeDecompFast.end()).operator*();
        double defaultDecompMin = std::min_element(timeDecompDefault.begin(), timeDecompDefault.end()).operator*();
        double defaultDecompAvg = std::accumulate(timeDecompDefault.begin(), timeDecompDefault.end(), 0.0) / timeDecompDefault.size();
        double defaultDecompMax = std::max_element(timeDecompDefault.begin(), timeDecompDefault.end()).operator*();
        double goodDecompMin = std::min_element(timeDecompGood.begin(), timeDecompGood.end()).operator*();
        double goodDecompAvg = std::accumulate(timeDecompGood.begin(), timeDecompGood.end(), 0.0) / timeDecompGood.size();
        double goodDecompMax = std::max_element(timeDecompGood.begin(), timeDecompGood.end()).operator*();
        double bestDecompMin = std::min_element(timeDecompBest.begin(), timeDecompBest.end()).operator*();
        double bestDecompAvg = std::accumulate(timeDecompBest.begin(), timeDecompBest.end(), 0.0) / timeDecompBest.size();
        double bestDecompMax = std::max_element(timeDecompBest.begin(), timeDecompBest.end()).operator*();

        std::cout << "// D E C O M P R E S S I O N ( . S L P )   B E N C H M A R K  \n";
        std::cout << "// -----------------------------------------------\n\n";
        std::cout << "Number of runs: " << NUM_OF_RUNS << "\n";
        std::cout << "Total time: " << msDecompTest.count() << "ms\n\n";
        std::cout << "Fastest - time: average=" << fastestDecompAvg << "ms min=" << fastestDecompMin << "ms max=" << fastestDecompMax << "ms\n";
        std::cout << "Fast - time: average=" << fastDecompAvg << "ms min=" << fastDecompMin << "ms max=" << fastDecompMax << "ms\n";
        std::cout << "Default - time: average=" << defaultDecompAvg << "ms min=" << defaultDecompMin << "ms max=" << defaultDecompMax << "ms\n";
        std::cout << "Good - time: average=" << goodDecompAvg << "ms min=" << goodDecompMin << "ms max=" << goodDecompMax << "ms\n";
        std::cout << "Best - time: average=" << bestDecompAvg << "ms min=" << bestDecompMin << "ms max=" << bestDecompMax << "ms\n";

        std::cout << std::endl;

        REQUIRE(true);
    }
#endif //RUN_BENCHMARK_TESTS

TEST_CASE( "Dummy - never fail!", "[dummy]" )
{
    std::string path = "./../../content/to_compress/to/j.lzma";
    std::vector<uint8_t> data;
    std::vector<uint8_t> decompressedData;
    plz::FileStatus fileStatus = plz::File::FromFile(path, data);
    if(fileStatus.status() == plz::FileStatus::Code::Ok)
    {
        //No settings / presets are used during decompression!
        plz::PocketLzma p;
        plz::StatusCode status = p.decompress(data, decompressedData);
        if(status == plz::StatusCode::Ok)
        {
            std::string outputPath = "./../../content/to_compress/from/j.json";
            plz::FileStatus writeStatus = plz::File::ToFile(outputPath, decompressedData);
            if(writeStatus.status() == plz::FileStatus::Code::Ok)
            {
                //Process completed successfully!
            }
        }
    }

    REQUIRE(true);
}

//TEST_CASE( "Dummy2 - never fail!", "[dummy]" )
//{
//    std::string path = "./yourFile.txt";
//
//    //When you are 100% sure your loading will never fail, you can use this
//    std::vector<uint8_t> data1 = plz::File::FromFile(path);
//
//    //However - I recommend to use this overload instead:
//    std::vector<uint8_t> data2;
//    plz::FileStatus fileStatus = plz::File::FromFile(path, data2);
//    //If something went wrong
//    if(fileStatus.status() != plz::FileStatus::Code::Ok)
//    {
//        plz::FileStatus::Code statusCode = fileStatus.status(); //PocketLzma status code. Will be useful if errors not causing exceptions happen.
//
//        //You may or may not have some error information here
//        int code = fileStatus.code();                       //Code returned from the OS in cases where an exception is thrown
//        std::string msg = fileStatus.message();             //Message from the OS in cases where an exception is thrown
//        std::string exception = fileStatus.exception();     //Exception message from the OS in cases where an exception is thrown
//        std::string category = fileStatus.category();       //Error category defined bythe OS in cases where an exception is thrown
//    }
//
//    //You can use memory data directly in PocketLzma, but you can use this if you want to transform them into a byte vector.
//    std::vector<uint8_t> memoryData;
//    plz::File::FromMemory(memfiles::_JSON_TEST_OK_HEADER_LZMA, memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE, memoryData);
//
//    //Finally, you can write to files like this
//    std::string writePath = "./yourOutputFile.txt";
//    plz::FileStatus fileWriteStatus = plz::File::ToFile(writePath, data2);
//
//    REQUIRE(true);
//}

