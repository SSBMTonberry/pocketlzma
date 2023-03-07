cmake_minimum_required(VERSION 3.11)
project(pocketlzma_tests_extra)

set(CMAKE_CXX_STANDARD 11)

include_directories(${PROJECT_SOURCE_DIR}/../external_libs/ ${PROJECT_SOURCE_DIR}/../src)

add_executable(pocketlzma_tests_extra lzma_c_test1.cpp lzma_c_test2.cpp)

if(LINUX)
    target_link_libraries(pocketlzma_tests_extra pthread)
elseif(APPLE)
    target_link_libraries(pocketlzma_tests_extra c++ pthread)
elseif(WINDOWS)
    if(MSVC)
        target_compile_options(pocketlzma_tests_extra PUBLIC /bigobj)
    else()
        target_compile_options(pocketlzma_tests_extra PUBLIC)
    endif()
endif()