//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_COMMON_H
#define POCKETLZMA_POCKETLZMA_COMMON_H

#include <vector>
#include <cstdint>

namespace plz
{
    //Constants
    const uint32_t LZMA2_MAX_BUFFER_SIZE {1 << 22};
    const uint32_t LZMA2_MAX_CHUNK_SIZE {1 << 16};
    const uint8_t LZMA2_MAX_HEADER_SIZE {6};
    const uint8_t LZMA2_MAX_HEADER_SIZE_UNCOMPRESSED {3};

    /*!
     * Inspired by lzma_ret
     */
    enum class StatusCode
    {
        /*! All good! */
        Ok = 0,
        /*!
         * End of stream was reached
		 *
		 * In encoder, LZMA_SYNC_FLUSH, LZMA_FULL_FLUSH, or
		 * LZMA_FINISH was finished. In decoder, this indicates
		 * that all the data was successfully decoded.
		 *
		 * In all cases, when LZMA_STREAM_END is returned, the last
		 * output bytes should be picked from strm->next_out.
         */
        StreamEnd = 1,

        /*!
         * Input stream has no integrity check
		 *
		 * This return value can be returned only if the
		 * LZMA_TELL_NO_CHECK flag was used when initializing
		 * the decoder. LZMA_NO_CHECK is just a warning, and
		 * the decoding can be continued normally.
		 *
		 * It is possible to call lzma_get_check() immediately after
		 * lzma_code has returned LZMA_NO_CHECK. The result will
		 * naturally be LZMA_CHECK_NONE, but the possibility to call
		 * lzma_get_check() may be convenient in some applications.
         */
        NoCheck = 2,

        /*!
         * Cannot calculate the integrity check
		 *
		 * The usage of this return value is different in encoders
		 * and decoders.
		 *
		 * Encoders can return this value only from the initialization
		 * function. If initialization fails with this value, the
		 * encoding cannot be done, because there's no way to produce
		 * output with the correct integrity check.
		 *
		 * Decoders can return this value only from lzma_code() and
		 * only if the LZMA_TELL_UNSUPPORTED_CHECK flag was used when
		 * initializing the decoder. The decoding can still be
		 * continued normally even if the check type is unsupported,
		 * but naturally the check will not be validated, and possible
		 * errors may go undetected.
		 *
		 * With decoder, it is possible to call lzma_get_check()
		 * immediately after lzma_code() has returned
		 * LZMA_UNSUPPORTED_CHECK. This way it is possible to find
		 * out what the unsupported Check ID was.
         */
        UnsupportedCheck = 3,

        /*!
         * Integrity check type is now available
		 *
		 * This value can be returned only by the lzma_code() function
		 * and only if the decoder was initialized with the
		 * LZMA_TELL_ANY_CHECK flag. LZMA_GET_CHECK tells the
		 * application that it may now call lzma_get_check() to find
		 * out the Check ID. This can be used, for example, to
		 * implement a decoder that accepts only files that have
		 * strong enough integrity check.
         */
        GetCheck = 4,

        /*!
         * Cannot allocate memory
		 *
		 * Memory allocation failed, or the size of the allocation
		 * would be greater than SIZE_MAX.
		 *
		 * Due to internal implementation reasons, the coding cannot
		 * be continued even if more memory were made available after
		 * LZMA_MEM_ERROR.
         */
        MemError = 5,

        /*!
         * Memory usage limit was reached
		 *
		 * Decoder would need more memory than allowed by the
		 * specified memory usage limit. To continue decoding,
		 * the memory usage limit has to be increased with
		 * lzma_memlimit_set().
         */
        MemLimitError = 6,

        /*!
         * File format not recognized
		 *
		 * The decoder did not recognize the input as supported file
		 * format. This error can occur, for example, when trying to
		 * decode .lzma format file with lzma_stream_decoder,
		 * because lzma_stream_decoder accepts only the .xz format.
         */
        FormatError = 7,

        /*!
         * Invalid or unsupported options
		 *
		 * Invalid or unsupported options, for example
		 *  - unsupported filter(s) or filter options; or
		 *  - reserved bits set in headers (decoder only).
		 *
		 * Rebuilding liblzma with more features enabled, or
		 * upgrading to a newer version of liblzma may help.
         */
        OptionsError = 8,

        /*!
         * Data is corrupt
		 *
		 * The usage of this return value is different in encoders
		 * and decoders. In both encoder and decoder, the coding
		 * cannot continue after this error.
		 *
		 * Encoders return this if size limits of the target file
		 * format would be exceeded. These limits are huge, thus
		 * getting this error from an encoder is mostly theoretical.
		 * For example, the maximum compressed and uncompressed
		 * size of a .xz Stream is roughly 8 EiB (2^63 bytes).
		 *
		 * Decoders return this error if the input data is corrupt.
		 * This can mean, for example, invalid CRC32 in headers
		 * or invalid check of uncompressed data.
         */
        DataError = 9,

        /*!
         * No progress is possible
		 *
		 * This error code is returned when the coder cannot consume
		 * any new input and produce any new output. The most common
		 * reason for this error is that the input stream being
		 * decoded is truncated or corrupt.
		 *
		 * This error is not fatal. Coding can be continued normally
		 * by providing more input and/or more output space, if
		 * possible.
		 *
		 * Typically the first call to lzma_code() that can do no
		 * progress returns LZMA_OK instead of LZMA_BUF_ERROR. Only
		 * the second consecutive call doing no progress will return
		 * LZMA_BUF_ERROR. This is intentional.
		 *
		 * With zlib, Z_BUF_ERROR may be returned even if the
		 * application is doing nothing wrong, so apps will need
		 * to handle Z_BUF_ERROR specially. The above hack
		 * guarantees that liblzma never returns LZMA_BUF_ERROR
		 * to properly written applications unless the input file
		 * is truncated or corrupt. This should simplify the
		 * applications a little.
         */
        BufError = 10,

        /*!
         * Programming error
		 *
		 * This indicates that the arguments given to the function are
		 * invalid or the internal state of the decoder is corrupt.
		 *   - Function arguments are invalid or the structures
		 *     pointed by the argument pointers are invalid
		 *     e.g. if strm->next_out has been set to NULL and
		 *     strm->avail_out > 0 when calling lzma_code().
		 *   - lzma_* functions have been called in wrong order
		 *     e.g. lzma_code() was called right after lzma_end().
		 *   - If errors occur randomly, the reason might be flaky
		 *     hardware.
		 *
		 * If you think that your code is correct, this error code
		 * can be a sign of a bug in liblzma. See the documentation
		 * how to report bugs.
         */
        ProgError = 11,

        /*! Undefined error */
        UndefinedError = 999
    };

    /*!
     * Compression modes
     *
     * This selects the function used to analyze the data produced by the match
     * finder.
     */
    enum class Mode
    {
        /*!
         * Fast compression
         *
         * Fast mode is usually at its best when combined with
         * a hash chain match finder.
         */
        Fast = 1,

        /*!
         * Normal compression
		 *
		 * This is usually notably slower than fast mode. Use this
		 * together with binary tree match finders to expose the
		 * full potential of the LZMA1 or LZMA2 encoder.
         */
        Normal = 2
    };

    /*!
     *
     * Match finder has major effect on both speed and compression ratio.
     * Usually hash chains are faster than binary trees.
     *
     * If you will use LZMA_SYNC_FLUSH often, the hash chains may be a better
     * choice, because binary trees get much higher compression ratio penalty
     * with LZMA_SYNC_FLUSH.
     *
     * The memory usage formulas are only rough estimates, which are closest to
     * reality when dict_size is a power of two. The formulas are  more complex
     * in reality, and can also change a little between liblzma versions. Use
     * lzma_raw_encoder_memusage() to get more accurate estimate of memory usage.
     */
    enum class MatchFinder
    {
        /*!
         * Hash Chain with 2- and 3-byte hashing
		 *
		 * Minimum nice_len: 3
		 *
		 * Memory usage:
		 *  - dict_size <= 16 MiB: dict_size * 7.5
		 *  - dict_size > 16 MiB: dict_size * 5.5 + 64 MiB
         */
        HC3 = 0x03,

        /*!
         * Hash Chain with 2-, 3-, and 4-byte hashing
		 *
		 * Minimum nice_len: 4
		 *
		 * Memory usage:
		 *  - dict_size <= 32 MiB: dict_size * 7.5
		 *  - dict_size > 32 MiB: dict_size * 6.5
         */
        HC4 = 0x04,

        /*!
         * Binary Tree with 2-byte hashing
		 *
		 * Minimum nice_len: 2
		 *
		 * Memory usage: dict_size * 9.5
         */
        BT2 = 0x12,

        /*!
         * Binary Tree with 2- and 3-byte hashing
		 *
		 * Minimum nice_len: 3
		 *
		 * Memory usage:
		 *  - dict_size <= 16 MiB: dict_size * 11.5
		 *  - dict_size > 16 MiB: dict_size * 9.5 + 64 MiB
         */
        BT3 = 0x13,

        /*!
         * Binary Tree with 2-, 3-, and 4-byte hashing
		 *
		 * Minimum nice_len: 4
		 *
		 * Memory usage:
		 *  - dict_size <= 32 MiB: dict_size * 11.5
		 *  - dict_size > 32 MiB: dict_size * 10.5
         */
        BT4 = 0x14
    };
}
#endif //POCKETLZMA_POCKETLZMA_COMMON_H
