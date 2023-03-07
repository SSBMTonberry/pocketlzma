cmake_minimum_required(VERSION 3.11)
project(pocketlzma_tests)

set(CMAKE_CXX_STANDARD 11)

# Uses the single header file when defined - comment out when creating PR: The cmake version in github doesn't seem to support it.
# add_compile_definitions(POCKETLZMA_UNIT_TEST_USE_SINGLE_HEADER)

include_directories(${PROJECT_SOURCE_DIR}/../external_libs/ ${PROJECT_SOURCE_DIR}/../src)

option(RUN_BENCHMARK_TESTS "Run benchmark tests" OFF)

configure_file (
        "${PROJECT_SOURCE_DIR}/PocketLzmaTestsConfig.h.in"
        "${PROJECT_SOURCE_DIR}/PocketLzmaTestsConfig.h"
)

add_executable(pocketlzma_tests tests_main.cpp tests_file.cpp)

if(LINUX)
    target_link_libraries(pocketlzma_tests pthread)
elseif(APPLE)
    target_link_libraries(pocketlzma_tests c++ pthread)
elseif(WINDOWS)
    if(MSVC)
        target_compile_options(pocketlzma_tests PUBLIC /bigobj)
    else()
        target_compile_options(pocketlzma_tests PUBLIC)
    endif()
endif()

unset(RUN_BENCHMARK_TESTS CACHE)