cmake_minimum_required(VERSION 3.11)
project(pocketlzma C CXX)

set(CMAKE_CXX_STANDARD 11)

set(POCKETLZMA_PROJECT_VERSION_MAJOR 0)
set(POCKETLZMA_PROJECT_VERSION_MINOR 9)
set(POCKETLZMA_PROJECT_VERSION_PATCH 0)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING
            "Choose the type of build, options are: Debug Release
RelWithDebInfo MinSizeRel."
            FORCE)
endif(NOT CMAKE_BUILD_TYPE)

option(BUILD_SHARED_LIBS "Build shared libraries." OFF)



#set(LZMA_SOURCE_FILES
#        src/external/lzma/C/7zAlloc.c
#        src/external/lzma/C/7zArcIn.c
#        src/external/lzma/C/7zBuf2.c
#        src/external/lzma/C/7zBuf.c
#        src/external/lzma/C/7zCrc.c
#        src/external/lzma/C/7zCrcOpt.c
#        src/external/lzma/C/7zDec.c
#        src/external/lzma/C/7zFile.c
#        src/external/lzma/C/7zStream.c
#        src/external/lzma/C/7zVersion.rc
#        src/external/lzma/C/Aes.c
#        src/external/lzma/C/AesOpt.c
#        src/external/lzma/C/Alloc.c
#        src/external/lzma/C/Bcj2.c
#        src/external/lzma/C/Bcj2Enc.c
#        src/external/lzma/C/Bra86.c
#        src/external/lzma/C/Bra.c
#        src/external/lzma/C/BraIA64.c
#        src/external/lzma/C/CpuArch.c
#        src/external/lzma/C/Delta.c
#        src/external/lzma/C/DllSecur.c
#        src/external/lzma/C/LzFind.c
#        src/external/lzma/C/LzFindMt.c
#        src/external/lzma/C/Lzma2Dec.c
#        src/external/lzma/C/Lzma2DecMt.c
#        src/external/lzma/C/Lzma2Enc.c
#        src/external/lzma/C/Lzma86Dec.c
#        src/external/lzma/C/Lzma86Enc.c
#        src/external/lzma/C/LzmaDec.c
#        src/external/lzma/C/LzmaEnc.c
#        src/external/lzma/C/LzmaLib.c
#        src/external/lzma/C/MtCoder.c
#        src/external/lzma/C/MtDec.c
#        src/external/lzma/C/Ppmd7.c
#        src/external/lzma/C/Ppmd7Dec.c
#        src/external/lzma/C/Ppmd7Enc.c
#        src/external/lzma/C/Sha256.c
#        src/external/lzma/C/Sort.c
#        src/external/lzma/C/Threads.c
#        src/external/lzma/C/Xz.c
#        src/external/lzma/C/XzCrc64.c
#        src/external/lzma/C/XzCrc64Opt.c
#        src/external/lzma/C/XzDec.c
#        src/external/lzma/C/XzEnc.c
#        src/external/lzma/C/XzIn.c
#        )

# set(LZMA_SOURCE_FILES
#         src/external/lzma/liblzma/common/alone_decoder.c
#         src/external/lzma/liblzma/common/alone_encoder.c
#         src/external/lzma/liblzma/common/auto_decoder.c
#         src/external/lzma/liblzma/common/block_buffer_decoder.c
#         src/external/lzma/liblzma/common/block_buffer_encoder.c
#         src/external/lzma/liblzma/common/block_decoder.c
#         src/external/lzma/liblzma/common/block_encoder.c
#         src/external/lzma/liblzma/common/block_header_decoder.c
#         src/external/lzma/liblzma/common/block_header_encoder.c
#         src/external/lzma/liblzma/common/block_util.c
#         src/external/lzma/liblzma/common/common.c
#         src/external/lzma/liblzma/common/easy_buffer_encoder.c
#         src/external/lzma/liblzma/common/easy_decoder_memusage.c
#         src/external/lzma/liblzma/common/easy_encoder.c
#         src/external/lzma/liblzma/common/easy_encoder_memusage.c
#         src/external/lzma/liblzma/common/easy_preset.c
#         src/external/lzma/liblzma/common/filter_buffer_decoder.c
#         src/external/lzma/liblzma/common/filter_buffer_encoder.c
#         src/external/lzma/liblzma/common/filter_common.c
#         src/external/lzma/liblzma/common/filter_decoder.c
#         src/external/lzma/liblzma/common/filter_encoder.c
#         src/external/lzma/liblzma/common/filter_flags_decoder.c
#         src/external/lzma/liblzma/common/filter_flags_encoder.c
#         src/external/lzma/liblzma/common/hardware_cputhreads.c
#         src/external/lzma/liblzma/common/hardware_physmem.c
#         src/external/lzma/liblzma/common/index.c
#         src/external/lzma/liblzma/common/index_decoder.c
#         src/external/lzma/liblzma/common/index_encoder.c
#         src/external/lzma/liblzma/common/index_hash.c
#         src/external/lzma/liblzma/common/outqueue.c
#         src/external/lzma/liblzma/common/stream_buffer_decoder.c
#         src/external/lzma/liblzma/common/stream_buffer_encoder.c
#         src/external/lzma/liblzma/common/stream_decoder.c
#         src/external/lzma/liblzma/common/stream_encoder.c
#         src/external/lzma/liblzma/common/stream_encoder_mt.c
#         src/external/lzma/liblzma/common/stream_flags_common.c
#         src/external/lzma/liblzma/common/stream_flags_decoder.c
#         src/external/lzma/liblzma/common/stream_flags_encoder.c
#         src/external/lzma/liblzma/common/vli_decoder.c
#         src/external/lzma/liblzma/common/vli_encoder.c
#         src/external/lzma/liblzma/common/vli_size.c
#
#         src/external/lzma/liblzma/lzma/fastpos_table.c
#         src/external/lzma/liblzma/lzma/fastpos_tablegen.c
#         src/external/lzma/liblzma/lzma/lzma2_decoder.c
#         src/external/lzma/liblzma/lzma/lzma2_encoder.c
#         src/external/lzma/liblzma/lzma/lzma_decoder.c
#         src/external/lzma/liblzma/lzma/lzma_encoder.c
#         src/external/lzma/liblzma/lzma/lzma_encoder_optimum_fast.c
#         src/external/lzma/liblzma/lzma/lzma_encoder_optimum_normal.c
#         src/external/lzma/liblzma/lzma/lzma_encoder_presets.c
#         )

# set(SOURCE_FILES src/pocketlzma.h src/interfaces/IDecompressor.hpp src/interfaces/ICompressor.hpp
#         src/pocketlzma_common.h src/main.cpp src/compress/Lzma2Compressor.hpp src/common/Lzma2Coder.hpp
#         src/common/LzmaOptions.hpp src/compress/LzmaCompressor.hpp src/common/CommonFunctions.hpp src/common/LzmaMF.hpp
#         src/common/LzmaMatch.hpp src/common/Lzma1Encoder.hpp src/common/RangeEncoder.hpp src/common/LengthEncoder.hpp
#         src/common/LzmaOptimal.hpp src/common/Price.hpp src/misc/MemoryBuffer.hpp src/misc/MemoryStream.hpp
#         src/misc/File.hpp src/common/Fastpos.hpp src/common/LzOptions.hpp src/pocketlzma_class.hpp src/Settings.hpp src/pocketlzma_common.hpp)

 set(SOURCE_FILES pocketlzma_program.cpp)

# Only activated for code completion, must be uncommented before release!
# add_library(pocketlzma ${SOURCE_FILES} ${LZMA_SOURCE_FILES}) #dummy build

add_subdirectory(tests)