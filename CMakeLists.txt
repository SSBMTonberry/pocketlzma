cmake_minimum_required(VERSION 3.11)
project(pocketlzma C CXX)

set(CMAKE_CXX_STANDARD 11)

set(POCKETLZMA_VERSION_MAJOR 1)
set(POCKETLZMA_VERSION_MINOR 1)
set(POCKETLZMA_VERSION_PATCH 0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

if(UNIX AND NOT APPLE)
    set(LINUX 1)
    set(WINDOWS 0)
    set(APPLE 0)
    set(MSVC 0)
    set(MINGW 0)
elseif(APPLE)
    link_directories(/usr/local/opt/llvm/lib/)
    set(LINUX 0)
    set(WINDOWS 0)
    set(APPLE 1)
    set(MSVC 0)
    set(MINGW 0)
elseif(WIN32)
    set(LINUX 0)
    set(WINDOWS 1)
    set(APPLE 0)
    if(MSVC)
        set(MSVC 1)
        set(MINGW 0)
    elseif(MINGW)
        set(MINGW 1)
        set(MSVC 0)
    endif()
else()
    set(LINUX 0)
    set(WINDOWS 0)
    set(APPLE 0)
    set(MSVC 0)
    set(MINGW 0)
endif()


option(BUILD_SHARED_LIBS "Build shared libraries." OFF)


configure_file (
        "${PROJECT_SOURCE_DIR}/PocketLzmaConfig.h.in"
        "${PROJECT_SOURCE_DIR}/src/PocketLzmaConfig.h"
)

set(SOURCE_FILES pocketlzma_program.cpp src/lzma_c/lzma_c.h)

add_executable(pocketlzma ${SOURCE_FILES})
if(LINUX)
    message("LINUX!")
    target_link_libraries(pocketlzma pthread)
    # target_compile_options(pocketlzma PRIVATE -Wall -Wextra -Werror)
elseif(APPLE)
    message("OSX!")
    target_link_libraries(pocketlzma c++ pthread)
    # target_compile_options(pocketlzma PRIVATE -Wall -Wextra -Werror)
elseif(WINDOWS)
    message("WINDOWS!")
    if(MSVC)
        message("MSVC!")
        target_compile_options(pocketlzma PUBLIC /bigobj /W4 /WX)
    else()
        message("NOT MSVC!")
        target_compile_options(pocketlzma PUBLIC)
    endif()
    #target_link_libraries(tileson_tests tileson)
endif()

add_subdirectory(tests)
add_subdirectory(tests_extra)