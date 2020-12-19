//
// Created by robin on 31.07.2019.
//

#define CATCH_CONFIG_MAIN
#include "../external_libs/catch.hpp"

//#define POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
//#define DISABLE_CPP17_FILESYSTEM

#ifdef POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER
    #include "../single_include/pocketlzma.hpp"
#else
    #include "../src/pocketlzma.h"
#endif

#include <memory>
#include <map>
#include <functional>

TEST_CASE( "Basic test", "[basic]" )
{
    std::set<uint32_t> s;
    s.insert(34);
    s.insert(12);
    s.insert(93);
    s.insert(34); //Should be ignored
    s.insert(34); //Should be ignored

    REQUIRE(true);
}