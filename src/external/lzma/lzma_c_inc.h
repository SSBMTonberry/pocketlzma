//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA_C_H
#define POCKETLZMA_LZMA_C_H

//Notes - LZMA-files
/*!
 * #include "Precomp.h"
 * #include "Lzma2Dec.h"
 * #include "Lzma2Dec.c"
 * #include "Lzma2DecMt.h"
 * #include "Alloc.h"
 * #include "MtDec.h"
 *
 *
 *
 */

namespace plz
{
    namespace c
    {
        //H
        //#include "liblzma/api/lzma.h"

        // C O M M O N
        // -------------------
        //#include "liblzma/common/lzma.h"

        //H
        #include "liblzma/common/alone_decoder.h"
        #include "liblzma/common/block_buffer_encoder.h"
        #include "liblzma/common/block_decoder.h"
        #include "liblzma/common/block_encoder.h"
        #include "liblzma/common/common.h"
        #include "liblzma/common/easy_preset.h"
        #include "liblzma/common/filter_common.h"
        #include "liblzma/common/filter_decoder.h"
        #include "liblzma/common/filter_encoder.h"
        #include "liblzma/common/index_encoder.h"
        #include "liblzma/common/index.h"
        #include "liblzma/common/memcmplen.h"
        #include "liblzma/common/outqueue.h"
        #include "liblzma/common/stream_decoder.h"
        #include "liblzma/common/stream_flags_common.h"


        // L Z M A
        // ----------------

        //H
        #include "liblzma/lzma/fastpos.h"
        #include "liblzma/lzma/lzma2_decoder.h"
        #include "liblzma/lzma/lzma2_encoder.h"
        #include "liblzma/lzma/lzma_common.h"
        #include "liblzma/lzma/lzma_decoder.h"
        #include "liblzma/lzma/lzma_encoder.h"
        #include "liblzma/lzma/lzma_encoder_private.h"

/*
 * total 212
drwxr-xr-x 3 robin robin   4096 des.  19 11:04 api
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 check
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 common
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 delta
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 lz
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 lzma
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 rangecoder
drwxr-xr-x 2 robin robin   4096 des.  19 11:04 simple
-rw-r--r-- 1 robin robin   3375 mars  17  2020 Makefile.am
-rw-r--r-- 1 robin robin    410 mars  17  2020 liblzma.pc.in
-rw-r--r-- 1 robin robin 160279 mars  17  2020 Makefile.in
-rw-r--r-- 1 robin robin   2338 mars  17  2020 liblzma.map
-rw-r--r-- 1 robin robin    287 mars  17  2020 liblzma_w32res.rc
-rw-r--r-- 1 robin robin   1509 mars  17  2020 validate_map.sh

./api:
total 40
drwxr-xr-x 2 robin robin  4096 des.  19 11:04 lzma
-rw-r--r-- 1 robin robin   409 mars  17  2020 Makefile.am
-rw-r--r-- 1 robin robin  9866 mars  17  2020 lzma.h
-rw-r--r-- 1 robin robin 18657 mars  17  2020 Makefile.in

./api/lzma:
total 188
-rw-r--r-- 1 robin robin 24858 mars  17  2020 base.h
-rw-r--r-- 1 robin robin  2630 mars  17  2020 bcj.h
-rw-r--r-- 1 robin robin 22107 mars  17  2020 block.h
-rw-r--r-- 1 robin robin  4255 mars  17  2020 check.h
-rw-r--r-- 1 robin robin 24844 mars  17  2020 container.h
-rw-r--r-- 1 robin robin  1865 mars  17  2020 delta.h
-rw-r--r-- 1 robin robin 16520 mars  17  2020 filter.h
-rw-r--r-- 1 robin robin  2604 mars  17  2020 hardware.h
-rw-r--r-- 1 robin robin 23491 mars  17  2020 index.h
-rw-r--r-- 1 robin robin  3914 mars  17  2020 index_hash.h
-rw-r--r-- 1 robin robin 14744 mars  17  2020 lzma12.h
-rw-r--r-- 1 robin robin  8253 mars  17  2020 stream_flags.h
-rw-r--r-- 1 robin robin  3497 mars  17  2020 version.h
-rw-r--r-- 1 robin robin  6546 mars  17  2020 vli.h

./check:
total 196
-rw-r--r-- 1 robin robin  2910 mars  17  2020 check.c
-rw-r--r-- 1 robin robin  2277 mars  17  2020 crc32_fast.c
-rw-r--r-- 1 robin robin  1044 mars  17  2020 crc32_small.c
-rw-r--r-- 1 robin robin   641 mars  17  2020 crc32_table.c
-rw-r--r-- 1 robin robin  2245 mars  17  2020 crc32_tablegen.c
-rw-r--r-- 1 robin robin  1640 mars  17  2020 crc64_fast.c
-rw-r--r-- 1 robin robin   966 mars  17  2020 crc64_small.c
-rw-r--r-- 1 robin robin   641 mars  17  2020 crc64_table.c
-rw-r--r-- 1 robin robin  1744 mars  17  2020 crc64_tablegen.c
-rw-r--r-- 1 robin robin  5405 mars  17  2020 sha256.c
-rw-r--r-- 1 robin robin  4718 mars  17  2020 check.h
-rw-r--r-- 1 robin robin 25758 mars  17  2020 crc32_table_be.h
-rw-r--r-- 1 robin robin 25758 mars  17  2020 crc32_table_le.h
-rw-r--r-- 1 robin robin 31882 mars  17  2020 crc64_table_be.h
-rw-r--r-- 1 robin robin 31882 mars  17  2020 crc64_table_le.h
-rw-r--r-- 1 robin robin   787 mars  17  2020 crc_macros.h
-rw-r--r-- 1 robin robin   970 mars  17  2020 Makefile.inc
-rw-r--r-- 1 robin robin  7228 mars  17  2020 crc32_x86.S
-rw-r--r-- 1 robin robin  6761 mars  17  2020 crc64_x86.S

./common:
total 396
-rw-r--r-- 1 robin robin  5662 mars  17  2020 alone_decoder.c
-rw-r--r-- 1 robin robin  3751 mars  17  2020 alone_encoder.c
-rw-r--r-- 1 robin robin  4902 mars  17  2020 auto_decoder.c
-rw-r--r-- 1 robin robin  2368 mars  17  2020 block_buffer_decoder.c
-rw-r--r-- 1 robin robin 10123 mars  17  2020 block_buffer_encoder.c
-rw-r--r-- 1 robin robin  6988 mars  17  2020 block_decoder.c
-rw-r--r-- 1 robin robin  5766 mars  17  2020 block_encoder.c
-rw-r--r-- 1 robin robin  3710 mars  17  2020 block_header_decoder.c
-rw-r--r-- 1 robin robin  3346 mars  17  2020 block_header_encoder.c
-rw-r--r-- 1 robin robin  2640 mars  17  2020 block_util.c
-rw-r--r-- 1 robin robin 10652 mars  17  2020 common.c
-rw-r--r-- 1 robin robin   840 mars  17  2020 easy_buffer_encoder.c
-rw-r--r-- 1 robin robin   668 mars  17  2020 easy_decoder_memusage.c
-rw-r--r-- 1 robin robin   678 mars  17  2020 easy_encoder.c
-rw-r--r-- 1 robin robin   654 mars  17  2020 easy_encoder_memusage.c
-rw-r--r-- 1 robin robin   727 mars  17  2020 easy_preset.c
-rw-r--r-- 1 robin robin  2510 mars  17  2020 filter_buffer_decoder.c
-rw-r--r-- 1 robin robin  1537 mars  17  2020 filter_buffer_encoder.c
-rw-r--r-- 1 robin robin  8706 mars  17  2020 filter_common.c
-rw-r--r-- 1 robin robin  4424 mars  17  2020 filter_decoder.c
-rw-r--r-- 1 robin robin  7345 mars  17  2020 filter_encoder.c
-rw-r--r-- 1 robin robin  1187 mars  17  2020 filter_flags_decoder.c
-rw-r--r-- 1 robin robin  1407 mars  17  2020 filter_flags_encoder.c
-rw-r--r-- 1 robin robin   526 mars  17  2020 hardware_cputhreads.c
-rw-r--r-- 1 robin robin   683 mars  17  2020 hardware_physmem.c
-rw-r--r-- 1 robin robin 35440 mars  17  2020 index.c
-rw-r--r-- 1 robin robin  8605 mars  17  2020 index_decoder.c
-rw-r--r-- 1 robin robin  5853 mars  17  2020 index_encoder.c
-rw-r--r-- 1 robin robin  8981 mars  17  2020 index_hash.c
-rw-r--r-- 1 robin robin  4586 mars  17  2020 outqueue.c
-rw-r--r-- 1 robin robin  2785 mars  17  2020 stream_buffer_decoder.c
-rw-r--r-- 1 robin robin  4056 mars  17  2020 stream_buffer_encoder.c
-rw-r--r-- 1 robin robin 13001 mars  17  2020 stream_decoder.c
-rw-r--r-- 1 robin robin  9856 mars  17  2020 stream_encoder.c
-rw-r--r-- 1 robin robin 30356 mars  17  2020 stream_encoder_mt.c
-rw-r--r-- 1 robin robin  1326 mars  17  2020 stream_flags_common.c
-rw-r--r-- 1 robin robin  2279 mars  17  2020 stream_flags_decoder.c
-rw-r--r-- 1 robin robin  2059 mars  17  2020 stream_flags_encoder.c
-rw-r--r-- 1 robin robin  2559 mars  17  2020 vli_decoder.c
-rw-r--r-- 1 robin robin  1935 mars  17  2020 vli_encoder.c
-rw-r--r-- 1 robin robin   630 mars  17  2020 vli_size.c
-rw-r--r-- 1 robin robin   601 mars  17  2020 alone_decoder.h
-rw-r--r-- 1 robin robin   763 mars  17  2020 block_buffer_encoder.h
-rw-r--r-- 1 robin robin   576 mars  17  2020 block_decoder.h
-rw-r--r-- 1 robin robin  1890 mars  17  2020 block_encoder.h
-rw-r--r-- 1 robin robin 10467 mars  17  2020 common.h
-rw-r--r-- 1 robin robin   921 mars  17  2020 easy_preset.h
-rw-r--r-- 1 robin robin  1258 mars  17  2020 filter_common.h
-rw-r--r-- 1 robin robin   617 mars  17  2020 filter_decoder.h
-rw-r--r-- 1 robin robin   732 mars  17  2020 filter_encoder.h
-rw-r--r-- 1 robin robin   584 mars  17  2020 index_encoder.h
-rw-r--r-- 1 robin robin  1978 mars  17  2020 index.h
-rw-r--r-- 1 robin robin  4819 mars  17  2020 memcmplen.h
-rw-r--r-- 1 robin robin  4998 mars  17  2020 outqueue.h
-rw-r--r-- 1 robin robin   599 mars  17  2020 stream_decoder.h
-rw-r--r-- 1 robin robin   883 mars  17  2020 stream_flags_common.h
-rw-r--r-- 1 robin robin  1881 mars  17  2020 Makefile.inc

./delta:
total 32
-rw-r--r-- 1 robin robin 1914 mars  17  2020 delta_common.c
-rw-r--r-- 1 robin robin 1910 mars  17  2020 delta_decoder.c
-rw-r--r-- 1 robin robin 3435 mars  17  2020 delta_encoder.c
-rw-r--r-- 1 robin robin  542 mars  17  2020 delta_common.h
-rw-r--r-- 1 robin robin  736 mars  17  2020 delta_decoder.h
-rw-r--r-- 1 robin robin  676 mars  17  2020 delta_encoder.h
-rw-r--r-- 1 robin robin  889 mars  17  2020 delta_private.h
-rw-r--r-- 1 robin robin  429 mars  17  2020 Makefile.inc

./lz:
total 84
-rw-r--r-- 1 robin robin  9130 mars  17  2020 lz_decoder.c
-rw-r--r-- 1 robin robin 17392 mars  17  2020 lz_encoder.c
-rw-r--r-- 1 robin robin 17379 mars  17  2020 lz_encoder_mf.c
-rw-r--r-- 1 robin robin  5841 mars  17  2020 lz_decoder.h
-rw-r--r-- 1 robin robin 10458 mars  17  2020 lz_encoder.h
-rw-r--r-- 1 robin robin  3539 mars  17  2020 lz_encoder_hash.h
-rw-r--r-- 1 robin robin  3252 mars  17  2020 lz_encoder_hash_table.h
-rw-r--r-- 1 robin robin   380 mars  17  2020 Makefile.inc

./lzma:
total 180
-rw-r--r-- 1 robin robin 33959 mars  17  2020 fastpos_table.c
-rw-r--r-- 1 robin robin  1268 mars  17  2020 fastpos_tablegen.c
-rw-r--r-- 1 robin robin  7576 mars  17  2020 lzma2_decoder.c
-rw-r--r-- 1 robin robin 10500 mars  17  2020 lzma2_encoder.c
-rw-r--r-- 1 robin robin 27321 mars  17  2020 lzma_decoder.c
-rw-r--r-- 1 robin robin 18518 mars  17  2020 lzma_encoder.c
-rw-r--r-- 1 robin robin  4402 mars  17  2020 lzma_encoder_optimum_fast.c
-rw-r--r-- 1 robin robin 23286 mars  17  2020 lzma_encoder_optimum_normal.c
-rw-r--r-- 1 robin robin  1763 mars  17  2020 lzma_encoder_presets.c
-rw-r--r-- 1 robin robin  3996 mars  17  2020 fastpos.h
-rw-r--r-- 1 robin robin   819 mars  17  2020 lzma2_decoder.h
-rw-r--r-- 1 robin robin  1186 mars  17  2020 lzma2_encoder.h
-rw-r--r-- 1 robin robin  6947 mars  17  2020 lzma_common.h
-rw-r--r-- 1 robin robin  1554 mars  17  2020 lzma_decoder.h
-rw-r--r-- 1 robin robin  1606 mars  17  2020 lzma_encoder.h
-rw-r--r-- 1 robin robin  3911 mars  17  2020 lzma_encoder_private.h
-rw-r--r-- 1 robin robin   873 mars  17  2020 Makefile.inc

./rangecoder:
total 36
-rw-r--r-- 1 robin robin  827 mars  17  2020 price_table.c
-rw-r--r-- 1 robin robin 1768 mars  17  2020 price_tablegen.c
-rw-r--r-- 1 robin robin 2013 mars  17  2020 price.h
-rw-r--r-- 1 robin robin 2317 mars  17  2020 range_common.h
-rw-r--r-- 1 robin robin 5009 mars  17  2020 range_decoder.h
-rw-r--r-- 1 robin robin 4616 mars  17  2020 range_encoder.h
-rw-r--r-- 1 robin robin  432 mars  17  2020 Makefile.inc

./simple:
total 64
-rw-r--r-- 1 robin robin 1686 mars  17  2020 arm.c
-rw-r--r-- 1 robin robin 1906 mars  17  2020 armthumb.c
-rw-r--r-- 1 robin robin 2740 mars  17  2020 ia64.c
-rw-r--r-- 1 robin robin 1936 mars  17  2020 powerpc.c
-rw-r--r-- 1 robin robin 8705 mars  17  2020 simple_coder.c
-rw-r--r-- 1 robin robin  982 mars  17  2020 simple_decoder.c
-rw-r--r-- 1 robin robin  983 mars  17  2020 simple_encoder.c
-rw-r--r-- 1 robin robin 2006 mars  17  2020 sparc.c
-rw-r--r-- 1 robin robin 3575 mars  17  2020 x86.c
-rw-r--r-- 1 robin robin 2171 mars  17  2020 simple_coder.h
-rw-r--r-- 1 robin robin  622 mars  17  2020 simple_decoder.h
-rw-r--r-- 1 robin robin  641 mars  17  2020 simple_encoder.h
-rw-r--r-- 1 robin robin 2211 mars  17  2020 simple_private.h
-rw-r--r-- 1 robin robin  833 mars  17  2020 Makefile.inc

 */
    }
}
#endif //POCKETLZMA_LZMA_C_H
