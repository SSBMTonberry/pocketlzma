//
// Created by robin on 29.12.2020.
//

#include "../external_libs/catch.hpp"

//#define POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER

#ifdef POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
 #include "../pocketlzma.hpp"
#else
 #include "../src/pocketlzma.h"
#endif

 TEST_CASE("Write random data to file - expect success", "[dumdum]")
 {
     std::string pathOut = "./../../content/to_compress/file_tests/file1.txt";
     std::vector<uint8_t> bytes {34, 32, 156, 4, 4, 4, 9};
     plz::FileStatus s = plz::File::ToFile(pathOut, bytes);

     REQUIRE(s.status() == plz::FileStatus::Code::Ok);
 }