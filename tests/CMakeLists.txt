cmake_minimum_required(VERSION 3.11)
project(pocketlzma_tests)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/../external_libs/ ${PROJECT_SOURCE_DIR}/../src)

option(RUN_BENCHMARK_TESTS "Run benchmark tests" OFF)

configure_file (
        "${PROJECT_SOURCE_DIR}/PocketLzmaTestsConfig.h.in"
        "${PROJECT_SOURCE_DIR}/PocketLzmaTestsConfig.h"
)

add_executable(pocketlzma_tests tests_main.cpp tests_file.cpp)

unset(RUN_BENCHMARK_TESTS CACHE)