cmake_minimum_required(VERSION 3.11)
project(pocketlzma C CXX)

set(CMAKE_CXX_STANDARD 11)

set(POCKETLZMA_VERSION_MAJOR 1)
set(POCKETLZMA_VERSION_MINOR 0)
set(POCKETLZMA_VERSION_PATCH 0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

option(BUILD_SHARED_LIBS "Build shared libraries." OFF)


configure_file (
        "${PROJECT_SOURCE_DIR}/PocketLzmaConfig.h.in"
        "${PROJECT_SOURCE_DIR}/src/PocketLzmaConfig.h"
)

set(SOURCE_FILES pocketlzma_program.cpp src/lzma_c/lzma_c.h)

add_executable(pocketlzma ${SOURCE_FILES})

add_subdirectory(tests)
add_subdirectory(tests_extra)