//
// Created by robin on 29.12.2020.
//

#include "../external_libs/catch.hpp"

//#define POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER

#define POCKETLZMA_LZMA_C_DEFINE //Do this only once

#ifdef POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
#include "../pocketlzma.hpp"
#else
#include "../src/pocketlzma.h"
#endif

TEST_CASE("Write random data to file - expect success", "[file]")
{
  std::string pathOut = "./../../content/file_tests/file1.txt";
  std::vector<uint8_t> bytes {34, 32, 156, 4, 4, 4, 9};
  plz::FileStatus s = plz::File::ToFile(pathOut, bytes);

  REQUIRE(s.status() == plz::FileStatus::Code::Ok);
}

TEST_CASE("Write random data to invalid directory - expect error", "[file]")
{
    std::string pathOut = "./../../content/file_tests_fail/file2.txt";
    std::vector<uint8_t> bytes {34, 32, 156, 4, 4, 4, 9};
    plz::FileStatus s = plz::File::ToFile(pathOut, bytes);

    REQUIRE(s.status() == plz::FileStatus::Code::FileWriteErrorFailBit);
}

TEST_CASE("Write random data to file then read - expect success", "[file]")
{
    std::string pathOut = "./../../content/file_tests/file3.txt";
    std::vector<uint8_t> bytes {34, 32, 156, 4, 4, 4, 9};

    std::vector<uint8_t> bytesRead;
    plz::FileStatus write = plz::File::ToFile(pathOut, bytes);
    REQUIRE(write.status() == plz::FileStatus::Code::Ok);

    plz::FileStatus read = plz::File::FromFile(pathOut, bytesRead);
    REQUIRE(read.status() == plz::FileStatus::Code::Ok);

    //Also read from a non-existant endpoint
    std::vector<uint8_t> bytesRead2;
    std::string pathInvalid = "./../../content/file_tests_fail/fileblobba.txt";
    plz::FileStatus read2 = plz::File::FromFile(pathInvalid, bytesRead2);
    REQUIRE(read2.status() == plz::FileStatus::Code::FileReadError);
}