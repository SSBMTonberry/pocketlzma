//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_H
#define POCKETLZMA_POCKETLZMA_H


/*** Start of inlined file: pocketlzma_common.h ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_COMMON_H
#define POCKETLZMA_POCKETLZMA_COMMON_H

#include <vector>
#include <cstdint>

namespace plz
{
	// T Y P E D E F S
	// ---------------------

	/*! Type of probabilities used with range coder
	 *
	 *  This needs to be at least 12-bit integer, so uint16_t is a logical choice.
	 *  However, on some architecture and compiler combinations, a bigger type
	 *  may give better speed, because the probability variables are accessed
	 *  a lot. On the other hand, bigger probability type increases cache
	 *  footprint, since there are 2 to 14 thousand probability variables in
	 *  LZMA (assuming the limit of lc + lp <= 4; with lc + lp <= 12 there
	 *  would be about 1.5 million variables).
	 *
	 *  With malicious files, the initialization speed of the LZMA decoder can
	 *  become important. In that case, smaller probability variables mean that
	 *  there is less bytes to write to RAM, which makes initialization faster.
	 *  With big probability type, the initialization can become so slow that it
	 *  can be a problem e.g. for email servers doing virus scanning.
	 *
	 *  I will be sticking to uint16_t unless some specific architectures
	 *  are *much* faster (20-50 %) with uint32_t.
	 */
	typedef uint16_t probability;

	// C O N S T A N T S
	// ---------------------

	//LZMA/LZMA2
	const uint32_t LZMA2_MAX_CHUNK_SIZE             {1 << 16};
	const uint32_t LZMA2_MAX_UNCOMPRESSED_SIZE      {1 << 22};
	const uint8_t LZMA2_MAX_HEADER_SIZE             {6};
	const uint8_t LZMA2_HEADER_SIZE_UNCOMPRESSED    {3};

	//LCLP (Literal Context / Literal Position)
	const uint8_t LZMA_LCLP_MIN                     {0};
	const uint8_t LZMA_LCLP_MAX                     {4};
	const uint8_t LZMA_LC_DEFAULT                   {3};

	//PB (Position Bits)
	const uint8_t LZMA_PB_MIN                       {0};
	const uint8_t LZMA_PB_MAX                       {4};
	const uint8_t LZMA_PB_DEFAULT                   {2};

	const uint8_t REPS                              {4};

	//vMatch length
	// Minimum length of a match is two bytes.
	const uint8_t MATCH_LEN_MIN                     {2};

	// Match length is encoded with 4, 5, or 10 bits.
	//
	// Length   Bits
	//  2-9      4 = Choice=0 + 3 bits
	// 10-17     5 = Choice=1 + Choice2=0 + 3 bits
	// 18-273   10 = Choice=1 + Choice2=1 + 8 bits
	const uint8_t LEN_LOW_BITS                          {3};
	const uint8_t LEN_LOW_SYMBOLS                       {1 << LEN_LOW_BITS};
	const uint8_t LEN_MID_BITS                          {3};
	const uint8_t LEN_MID_SYMBOLS                       {1 << LEN_MID_BITS};
	const uint8_t LEN_HIGH_BITS                         {8};
	const uint16_t LEN_HIGH_SYMBOLS                     {1 << LEN_HIGH_BITS};
	const uint16_t LEN_SYMBOLS                          {LEN_LOW_SYMBOLS + LEN_MID_SYMBOLS + LEN_HIGH_SYMBOLS};

	// Maximum length of a match is 273 which is a result of the encoding
	// described above.
	const uint16_t MATCH_LEN_MAX                        {MATCH_LEN_MIN + LEN_SYMBOLS - 1};

	/// Each literal coder is divided in three sections:
	///   - 0x001-0x0FF: Without match byte
	///   - 0x101-0x1FF: With match byte; match bit is 0
	///   - 0x201-0x2FF: With match byte; match bit is 1
	///
	/// Match byte is used when the previous LZMA symbol was something else than
	/// a literal (that is, it was some kind of match).
	const uint16_t LITERAL_CODER_SIZE                   {0x300};

	/// Maximum number of literal coders
	const uint16_t LITERAL_CODERS_MAX                   {1 << LZMA_LCLP_MAX};

	/// Total number of states
	const uint8_t STATES                                {12};

	/// The lowest 7 states indicate that the previous state was a literal.
	const uint8_t LIT_STATES                            {7};

	/// Maximum number of position states. A position state is the lowest pos bits
	/// number of bits of the current uncompressed offset. In some places there
	/// are different sets of probabilities for different pos states.
	const uint16_t  POS_STATES_MAX                      {1 << LZMA_PB_MAX};

	////////////////////
	// Match distance //
	////////////////////

	// Different sets of probabilities are used for match distances that have very
	// short match length: Lengths of 2, 3, and 4 bytes have a separate set of
	// probabilities for each length. The matches with longer length use a shared
	// set of probabilities.
	const uint8_t DIST_STATES                           {4};

	// The highest two bits of a match distance (distance slot) are encoded
	// using six bits. See fastpos.h for more explanation.
	const uint8_t DIST_SLOT_BITS                        {6};
	const uint16_t DIST_SLOTS                           {1 << DIST_SLOT_BITS};

	// Match distances up to 127 are fully encoded using probabilities. Since
	// the highest two bits (distance slot) are always encoded using six bits,
	// the distances 0-3 don't need any additional bits to encode, since the
	// distance slot itself is the same as the actual distance. DIST_MODEL_START
	// indicates the first distance slot where at least one additional bit is
	// needed.
	const uint8_t DIST_MODEL_START                      {4};

	// Match distances greater than 127 are encoded in three pieces:
	//   - distance slot: the highest two bits
	//   - direct bits: 2-26 bits below the highest two bits
	//   - alignment bits: four lowest bits
	//
	// Direct bits don't use any probabilities.
	//
	// The distance slot value of 14 is for distances 128-191 (see the table in
	// fastpos.h to understand why).
	const uint8_t DIST_MODEL_END                        {14};

	// Distance slots that indicate a distance <= 127.
	const uint8_t FULL_DISTANCES_BITS                   {DIST_MODEL_END / 2};
	const uint16_t FULL_DISTANCES                       {1 << FULL_DISTANCES_BITS};

	// For match distances greater than 127, only the highest two bits and the
	// lowest four bits (alignment) is encoded using probabilities.
	const uint8_t ALIGN_BITS                            {4};
	const uint8_t ALIGN_SIZE                            {1 << ALIGN_BITS};
	const uint8_t ALIGN_MASK                            {ALIGN_SIZE - 1};

	// Optimal - Number of entries in the optimum array.
	const uint16_t OPTS                                 {1 << 12};

	//RangeEncoder (RC)

	const uint8_t RC_SHIFT_BITS                         {8};
	const uint8_t RC_TOP_BITS                           {24};
	const uint32_t RC_TOP_VALUE                         {1 << RC_TOP_BITS};
	const uint8_t RC_BIT_MODEL_TOTAL_BITS               {11};
	const uint32_t RC_BIT_MODEL_TOTAL                   {1 << RC_BIT_MODEL_TOTAL_BITS};
	const uint8_t RC_MOVE_BITS                          {5};

	/*! Number of bytes that a single encoding loop in lzma_lzma_encode() can
	 *  consume from the dictionary. This limit comes from lzma_lzma_optimum()
	 *  and may need to be updated if that function is significantly modified.
	 */
	const uint16_t LOOP_INPUT_MAX                      {OPTS + 1};

	const uint8_t FASTPOS_BITS                         {13};

	/*!
	 * Inspired by lzma_ret
	 */
	enum class StatusCode : uint32_t
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

		// P O C K E T L Z M A   S P E C I F I C
		// ----------------------------------------
		// 100-110 - Header/data validation
		UncompressedSizeIsZero                  = 100,
		UncompressedSizeTooLargeLzma2           = 101,
		CompressedSizeIsZero                    = 102,
		CompressedSizeIsLargerThanMaxChunkLzma2 = 103,
		ErrorInLclppbCheckOnLzmaOptions         = 104,
		InvalidLclppbByteValue                  = 105,

		// 200 - LZMA1 Encoding
		LzmaEncodingLiteralInitError            = 200,

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

	/*!
	 * The `action' argument for lzma_code()
	 *
	 * After the first use of LZMA_SYNC_FLUSH, LZMA_FULL_FLUSH, LZMA_FULL_BARRIER,
	 * or LZMA_FINISH, the same `action' must is used until lzma_code() returns
	 * LZMA_STREAM_END. Also, the amount of input (that is, strm->avail_in) must
	 * not be modified by the application until lzma_code() returns
	 * LZMA_STREAM_END. Changing the `action' or modifying the amount of input
	 * will make lzma_code() return LZMA_PROG_ERROR.
	 */
	enum class LzmaAction
	{
		/*!
		 * Continue coding
		 *
		 * Encoder: Encode as much input as possible. Some internal
		 * buffering will probably be done (depends on the filter
		 * chain in use), which causes latency: the input used won't
		 * usually be decodeable from the output of the same
		 * lzma_code() call.
		 *
		 * Decoder: Decode as much input as possible and produce as
		 * much output as possible.
		 */
		Run = 0,

		/*!
		 * Make all the input available at output
		 *
		 * Normally the encoder introduces some latency.
		 * LZMA_SYNC_FLUSH forces all the buffered data to be
		 * available at output without resetting the internal
		 * state of the encoder. This way it is possible to use
		 * compressed stream for example for communication over
		 * network.
		 *
		 * Only some filters support LZMA_SYNC_FLUSH. Trying to use
		 * LZMA_SYNC_FLUSH with filters that don't support it will
		 * make lzma_code() return LZMA_OPTIONS_ERROR. For example,
		 * LZMA1 doesn't support LZMA_SYNC_FLUSH but LZMA2 does.
		 *
		 * Using LZMA_SYNC_FLUSH very often can dramatically reduce
		 * the compression ratio. With some filters (for example,
		 * LZMA2), fine-tuning the compression options may help
		 * mitigate this problem significantly (for example,
		 * match finder with LZMA2).
		 *
		 * Decoders don't support LZMA_SYNC_FLUSH.
		 */
		SyncFlush = 1,

		/*!
		 * Finish encoding of the current Block
		 *
		 * All the input data going to the current Block must have
		 * been given to the encoder (the last bytes can still be
		 * pending in *next_in). Call lzma_code() with LZMA_FULL_FLUSH
		 * until it returns LZMA_STREAM_END. Then continue normally
		 * with LZMA_RUN or finish the Stream with LZMA_FINISH.
		 *
		 * This action is currently supported only by Stream encoder
		 * and easy encoder (which uses Stream encoder). If there is
		 * no unfinished Block, no empty Block is created.
		 */
		FullFlush = 2,

		/*!
		 * Finish encoding of the current Block
		 *
		 * This is like LZMA_FULL_FLUSH except that this doesn't
		 * necessarily wait until all the input has been made
		 * available via the output buffer. That is, lzma_code()
		 * might return LZMA_STREAM_END as soon as all the input
		 * has been consumed (avail_in == 0).
		 *
		 * LZMA_FULL_BARRIER is useful with a threaded encoder if
		 * one wants to split the .xz Stream into Blocks at specific
		 * offsets but doesn't care if the output isn't flushed
		 * immediately. Using LZMA_FULL_BARRIER allows keeping
		 * the threads busy while LZMA_FULL_FLUSH would make
		 * lzma_code() wait until all the threads have finished
		 * until more data could be passed to the encoder.
		 *
		 * With a lzma_stream initialized with the single-threaded
		 * lzma_stream_encoder() or lzma_easy_encoder(),
		 * LZMA_FULL_BARRIER is an alias for LZMA_FULL_FLUSH.
		 */
		FullBarrier = 4,

		/*!
		 * Finish the coding operation
		 *
		 * All the input data must have been given to the encoder
		 * (the last bytes can still be pending in next_in).
		 * Call lzma_code() with LZMA_FINISH until it returns
		 * LZMA_STREAM_END. Once LZMA_FINISH has been used,
		 * the amount of input must no longer be changed by
		 * the application.
		 *
		 * When decoding, using LZMA_FINISH is optional unless the
		 * LZMA_CONCATENATED flag was used when the decoder was
		 * initialized. When LZMA_CONCATENATED was not used, the only
		 * effect of LZMA_FINISH is that the amount of input must not
		 * be changed just like in the encoder.
		 */
		Finish = 3
	};

	/*! This enum is used to track which events have occurred most recently and
	 *  in which order. This information is used to predict the next event.
	 *
	 *  Events:
	 *   - Literal: One 8-bit byte
	 *   - Match: Repeat a chunk of data at some distance
	 *   - Long repeat: Multi-byte match at a recently seen distance
	 *   - Short repeat: One-byte repeat at a recently seen distance
	 *
	 *  The event names are in from STATE_oldest_older_previous. REP means
	 *  either short or long repeated match, and NONLIT means any non-literal.
	 */
	 enum class LzmaState
	 {
		 LitLit,
		 MatchLitLit,
		 RepLitLit,
		 ShortRepLitLit,
		 MatchLit,
		 RepLit,
		 ShortRepLit,
		 LitMatch,
		 LitLongRep,
		 LitShortRep,
		 NonLitMatch,
		 NonLitRep
	 };
}
#endif //POCKETLZMA_POCKETLZMA_COMMON_H

/*** End of inlined file: pocketlzma_common.h ***/


/*** Start of inlined file: CommonFunctions.hpp ***/
//
// Created by robin on 24.12.2020.
//

#ifndef POCKETLZMA_COMMONFUNCTIONS_HPP
#define POCKETLZMA_COMMONFUNCTIONS_HPP

namespace plz
{
	// These allow helping the compiler in some often-executed branches, whose
	// result is almost always the same.
	#ifdef __GNUC__
	#	define likely(expr) __builtin_expect(expr, true)
	#	define unlikely(expr) __builtin_expect(expr, false)
	#else
	#	define likely(expr) (expr)
	#	define unlikely(expr) (expr)
	#endif

	#define change_pair(small_dist, big_dist) \
	(((big_dist) >> 7) > (small_dist))

	/********************
	 * GNU C extensions *
	 ********************/

	/*
	 * GNU C extensions are used conditionally in the public API. It doesn't
	 * break anything if these are sometimes enabled and sometimes not, only
	 * affects warnings and optimizations.
	 */
	#if defined(__GNUC__) && __GNUC__ >= 3
	#	ifndef lzma_attribute
	#		define lzma_attribute(attr) __attribute__(attr)
	#	endif

		/* warn_unused_result was added in GCC 3.4. */
	#	ifndef lzma_attr_warn_unused_result
	#		if __GNUC__ == 3 && __GNUC_MINOR__ < 4
	#			define lzma_attr_warn_unused_result
	#		endif
	#	endif

	#else
		#	ifndef lzma_attribute
	#		define lzma_attribute(attr)
	#	endif
	#endif

	#ifndef lzma_attr_pure
	#	define lzma_attr_pure lzma_attribute((__pure__))
	#endif

	#ifndef lzma_attr_const
	#	define lzma_attr_const lzma_attribute((__const__))
	#endif

	#ifndef lzma_attr_warn_unused_result
	#	define lzma_attr_warn_unused_result \
			lzma_attribute((__warn_unused_result__))
	#endif

	/*! NOTE: Avoid using MIN() and MAX(), because even conditionally defining
	 *  those macros can cause some portability trouble, since on some systems
	 *  the system headers insist defining their own versions.
	 */
	#define my_min(x, y) ((x) < (y) ? (x) : (y))
	#define my_max(x, y) ((x) > (y) ? (x) : (y))

	/*! Macro to compare if the first two bytes in two buffers differ. This is
	 *  needed in lzma_lzma_optimum_*() to test if the match is at least
	 *  MATCH_LEN_MIN bytes. Unaligned access gives tiny gain so there's no
	 *  reason to not use it when it is supported.
	 */
	#define not_equal_16(a, b) \
		((a)[0] != (b)[0] || (a)[1] != (b)[1])

	/// Locate the literal coder for the next literal byte. The choice depends on
	///   - the lowest literal_pos_bits bits of the position of the current
	///     byte; and
	///   - the highest literal_context_bits bits of the previous byte.
	#define literal_subcoder(probs, lc, lp_mask, pos, prev_byte) \
		((probs)[(((pos) & (lp_mask)) << (lc)) \
				+ ((uint32_t)(prev_byte) >> (8U - (lc)))])

	/// Indicate that the latest state was a literal.
	#define update_literal(state) \
		state = ((state) <= STATE_SHORTREP_LIT_LIT \
				? STATE_LIT_LIT \
				: ((state) <= STATE_LIT_SHORTREP \
					? (state) - 3 \
					: (state) - 6))

	/// Indicate that the latest state was a match.
	//#define update_match(state) \
	//    state = ((state) < LIT_STATES ? STATE_LIT_MATCH : STATE_NONLIT_MATCH)

	/// Indicate that the latest state was a long repeated match.
	//#define update_long_rep(state) \
	//    state = ((state) < LIT_STATES ? STATE_LIT_LONGREP : STATE_NONLIT_REP)

	/// Indicate that the latest state was a short match.
	//#define update_short_rep(state) \
	//    state = ((state) < LIT_STATES ? STATE_LIT_SHORTREP : STATE_NONLIT_REP)

	/// Test if the previous state was a literal.
	//#define is_literal_state(state) \
	//    ((state) < LIT_STATES)

	/*! Indicate that the latest state was a match. */
	static inline void UpdateMatch(LzmaState &state)
	{
		state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitMatch: LzmaState::NonLitMatch;
	}

	/*! Indicate that the latest state was a long repeated match. */
	static inline void UpdateLongRep(LzmaState &state)
	{
		state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitLongRep: LzmaState::NonLitRep;
	}

	/*! Indicate that the latest state was a short match. */
	static inline void UpdateShortRep(LzmaState &state)
	{
		state = ((uint8_t)state < LIT_STATES) ? LzmaState::LitShortRep: LzmaState::NonLitRep;
	}

	/*! Test if the previous state was a literal. */
	static inline bool IsLiteralState(LzmaState state)
	{
		return (uint8_t)state < LIT_STATES;
	}

	/*! Resets the probability so that both 0 and 1 have probability of 50 % */
	static inline void BitReset(probability &prob)
	{
		prob = RC_BIT_MODEL_TOTAL >> 1;
	}

	/*! This does the same for a complete bit tree. (A tree represented as an array.) */
	static inline void BittreeReset(probability *probs, uint32_t bit_levels)
	{
		for (uint32_t bt_i = 0; bt_i < (1 << (bit_levels)); ++bt_i)
		    BitReset((probs)[bt_i]);
	}

	/*! Find out how many equal bytes the two buffers have.
	 *
	 * \param      buf1    First buffer
	 * \param      buf2    Second buffer
	 * \param      len     How many bytes have already been compared and will
	 *                     be assumed to match
	 * \param      limit   How many bytes to compare at most, including the
	 *                     already-compared bytes. This must be significantly
	 *                     smaller than UINT32_MAX to avoid integer overflows.
	 *                     Up to LZMA_MEMCMPLEN_EXTRA bytes may be read past
	 *                     the specified limit from both buf1 and buf2.
	 *
	 * \return     Number of equal bytes in the buffers is returned.
	 *             This is always at least len and at most limit.
	 *
	 * \note       LZMA_MEMCMPLEN_EXTRA defines how many extra bytes may be read.
	 *             It's rounded up to 2^n. This extra amount needs to be
	 *             allocated in the buffers being used. It needs to be
	 *             initialized too to keep Valgrind quiet.
	 */
	static inline uint32_t lzma_attribute((__always_inline__))
			LzmaMemcmplen(const uint8_t *buf1, const uint8_t *buf2, uint32_t len, uint32_t limit)
	{
		assert(len <= limit);
		assert(limit <= UINT32_MAX / 2);

		// Simple portable version that doesn't use unaligned access.
		# define LZMA_MEMCMPLEN_EXTRA 0
		while (len < limit && buf1[len] == buf2[len])
			++len;

		return len;

	}

	extern const uint8_t lzma_fastpos[1 << FASTPOS_BITS];

#define fastpos_shift(extra, n) \
	((extra) + (n) * (FASTPOS_BITS - 1))

#define fastpos_limit(extra, n) \
	(UINT32_C(1) << (FASTPOS_BITS + fastpos_shift(extra, n)))

#define fastpos_result(dist, extra, n) \
	(uint32_t)(lzma_fastpos[(dist) >> fastpos_shift(extra, n)]) \
			+ 2 * fastpos_shift(extra, n)

	static inline uint32_t GetDistSlot(uint32_t dist)
	{
		// If it is small enough, we can pick the result directly from
		// the precalculated table.
		if (dist < fastpos_limit(0, 0))
			return lzma_fastpos[dist];

		if (dist < fastpos_limit(0, 1))
			return fastpos_result(dist, 0, 1);

		return fastpos_result(dist, 0, 2);
	}

	/*! Uses FULL_DISTANCES_BITS */
	static inline uint32_t GetDistSlot2(uint32_t dist)
	{
		assert(dist >= FULL_DISTANCES);

		if (dist < fastpos_limit(FULL_DISTANCES_BITS - 1, 0))
			return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 0);

		if (dist < fastpos_limit(FULL_DISTANCES_BITS - 1, 1))
			return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 1);

		return fastpos_result(dist, FULL_DISTANCES_BITS - 1, 2);
	}
}

#endif //POCKETLZMA_COMMONFUNCTIONS_HPP

/*** End of inlined file: CommonFunctions.hpp ***/


/*** Start of inlined file: Price.hpp ***/
//
// Created by robin on 26.12.2020.
// Probability price calculation
//
//

#ifndef POCKETLZMA_PRICE_HPP
#define POCKETLZMA_PRICE_HPP

namespace plz
{
	const uint8_t  RC_MOVE_REDUCING_BITS         {4};
	const uint8_t  RC_BIT_PRICE_SHIFT_BITS       {4};
	const uint32_t RC_PRICE_TABLE_SIZE           {RC_BIT_MODEL_TOTAL >> RC_MOVE_REDUCING_BITS};

	const uint32_t RC_INFINITY_PRICE             {1 << 30};

	/*! Collection of probability price calculations (static)
	 *  Based on price.h
	 */
	class Price
	{
		public:
			Price() = delete;

			/*! rc_bit_price */
			static inline uint32_t RcBitPrice(const probability prob, const uint32_t bit);
			/*! rc_bit_0_price */
			static inline uint32_t RcBit0Price(const probability prob);
			/*! rc_bit_1_price */
			static inline uint32_t RcBit1Price(const probability prob);
			/*! rc_bittree_price */
			static inline uint32_t RcBittreePrice(const probability *const probs,
							 const uint32_t bit_levels, uint32_t symbol);
			/*! rc_bittree_reverse_price */
			static inline uint32_t RcBittreeReversePrice(const probability *const probs,
									 uint32_t bit_levels, uint32_t symbol);

			/*! rc_direct_price */
			static inline uint32_t RcDirectPrice(const uint32_t bits);

		private:
			static const uint8_t lzma_rc_prices[RC_PRICE_TABLE_SIZE];

	};

	uint32_t Price::RcBitPrice(const probability prob, const uint32_t bit)
	{
		return lzma_rc_prices[(prob ^ ((0 - bit)
									   & (RC_BIT_MODEL_TOTAL - 1))) >> RC_MOVE_REDUCING_BITS];
	}

	uint32_t Price::RcBit0Price(const probability prob)
	{
		return lzma_rc_prices[prob >> RC_MOVE_REDUCING_BITS];
	}

	uint32_t Price::RcBit1Price(const probability prob)
	{
		return lzma_rc_prices[(prob ^ (RC_BIT_MODEL_TOTAL - 1))
				>> RC_MOVE_REDUCING_BITS];
	}

	uint32_t Price::RcBittreePrice(const probability *const probs, const uint32_t bit_levels, uint32_t symbol)
	{
		uint32_t price = 0;
		symbol += 1 << bit_levels;

		do {
			const uint32_t bit = symbol & 1;
			symbol >>= 1;
			price += RcBitPrice(probs[symbol], bit);
		} while (symbol != 1);

		return price;
	}

	uint32_t Price::RcBittreeReversePrice(const probability *const probs, uint32_t bit_levels, uint32_t symbol)
	{
		uint32_t price = 0;
		uint32_t model_index = 1;

		do {
			const uint32_t bit = symbol & 1;
			symbol >>= 1;
			price += RcBitPrice(probs[model_index], bit);
			model_index = (model_index << 1) + bit;
		} while (--bit_levels != 0);

		return price;
	}

	uint32_t Price::RcDirectPrice(const uint32_t bits)
	{
		return bits << RC_BIT_PRICE_SHIFT_BITS;
	}
}

#endif //POCKETLZMA_PRICE_HPP

/*** End of inlined file: Price.hpp ***/


/*** Start of inlined file: RangeEncoder.hpp ***/
//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_RANGEENCODER_HPP
#define POCKETLZMA_RANGEENCODER_HPP

namespace plz
{
	class RangeEncoder
	{
		public:
			/*! Maximum number of symbols that can be put pending into lzma_range_encoder
				structure between calls to lzma_rc_encode(). For LZMA, 52+5 is enough
				(match with big distance and length followed by range encoder flush). */
			static const uint8_t RC_SYMBOLS_MAX {58};
			enum class Symbol : uint32_t
			{
				Bit0,
				Bit1,
				Direct0,
				Direct1,
				Flush
			};

			RangeEncoder() = default;

			uint64_t low;
			uint64_t cacheSize;
			uint32_t range;
			uint8_t cache;

			/// Number of symbols in the tables
			size_t count;

			/// rc_encode()'s position in the tables
			size_t pos;

			/// Symbols to encode
			Symbol symbols[RC_SYMBOLS_MAX];

			/// Probabilities associated with RC_BIT_0 or RC_BIT_1
			probability *probs[RC_SYMBOLS_MAX];

			//Functions

			/*! rc_reset */
			inline void reset();
			/*! rc_bit */
			inline void bit(probability *prob, uint32_t bit);
			/*! rc_bittree */
			inline void bittree(probability *probabilities, uint32_t bit_count, uint32_t symbol);
			/*! rc_bittree_reverse */
			inline void bittreeReverse(probability *probabilities, uint32_t bit_count, uint32_t symbol);
			/*! rc_direct */
			inline void direct(uint32_t value, uint32_t bit_count);
			/*! rc_flush */
			inline void flush();
			/*! rc_shift_low */
			inline bool shiftLow(uint8_t *out, size_t *out_pos, size_t out_size);
			/*! rc_encode */
			inline bool encode(uint8_t *out, size_t *out_pos, size_t out_size);
			/*! rc_pending */
			inline uint64_t pending();

			/*! Symbol to int converter*/
			inline uint32_t sint(Symbol s);
			/*! Symbol to int converter*/
			inline Symbol intToSymbol(uint32_t i);

	};

	void RangeEncoder::reset()
	{
		low = 0;
		cacheSize = 1;
		range = UINT32_MAX;
		cache = 0;
		count = 0;
		pos = 0;
	}

	void RangeEncoder::bit(probability *prob, uint32_t bit)
	{
		symbols[count] = (Symbol) bit;
		probs[count] = prob;
		++count;
	}

	void RangeEncoder::bittree(probability *probabilities, uint32_t bit_count, uint32_t symbol)
	{
		uint32_t model_index = 1;

		do {
			const uint32_t b = (symbol >> --bit_count) & 1;
			bit(&probabilities[model_index], b);
			model_index = (model_index << 1) + b;
		} while (bit_count != 0);
	}

	void RangeEncoder::bittreeReverse(probability *probabilities, uint32_t bit_count, uint32_t symbol)
	{
		uint32_t model_index = 1;

		do {
			const uint32_t b = symbol & 1;
			symbol >>= 1;
			bit(&probabilities[model_index], b);
			model_index = (model_index << 1) + b;
		} while (--bit_count != 0);
	}

	void RangeEncoder::direct(uint32_t value, uint32_t bit_count)
	{
		do {
			symbols[count++] = intToSymbol(sint(Symbol::Direct0) + ((value >> --bit_count) & 1));
		} while (bit_count != 0);
	}

	void RangeEncoder::flush()
	{
		for (size_t i = 0; i < 5; ++i)
			symbols[count++] = Symbol::Flush;
	}

	bool RangeEncoder::shiftLow(uint8_t *out, size_t *out_pos, size_t out_size)
	{
		if ((uint32_t)(low) < (uint32_t)(0xFF000000)
			|| (uint32_t)(low >> 32) != 0) {
			do {
				if (*out_pos == out_size)
					return true;

				out[*out_pos] = cache + (uint8_t)(low >> 32);
				++*out_pos;
				cache = 0xFF;

			} while (--cacheSize != 0);

			cache = (low >> 24) & 0xFF;
		}

		++cacheSize;
		low = (low & 0x00FFFFFF) << RC_SHIFT_BITS;

		return false;
	}

	bool RangeEncoder::encode(uint8_t *out, size_t *out_pos, size_t out_size)
	{
		assert(count <= RC_SYMBOLS_MAX);

		while (pos < count) {
			// Normalize
			if (range < RC_TOP_VALUE) {
				if (shiftLow(out, out_pos, out_size))
					return true;

				range <<= RC_SHIFT_BITS;
			}

			// Encode a bit
			switch (symbols[pos]) {
				case Symbol::Bit0: {
					probability prob = *probs[pos];
					range = (range >> RC_BIT_MODEL_TOTAL_BITS)
								* prob;
					prob += (RC_BIT_MODEL_TOTAL - prob) >> RC_MOVE_BITS;
					*probs[pos] = prob;
					break;
				}

				case Symbol::Bit1: {
					probability prob = *probs[pos];
					const uint32_t bound = prob * (range
							>> RC_BIT_MODEL_TOTAL_BITS);
					low += bound;
					range -= bound;
					prob -= prob >> RC_MOVE_BITS;
					*probs[pos] = prob;
					break;
				}

				case Symbol::Direct0:
					range >>= 1;
					break;

				case Symbol::Direct1:
					range >>= 1;
					low += range;
					break;

				case Symbol::Flush:
					// Prevent further normalizations.
					range = UINT32_MAX;

					// Flush the last five bytes (see rc_flush()).
					do {
						if (shiftLow(out, out_pos, out_size))
							return true;
					} while (++pos < count);

					// Reset the range encoder so we are ready to continue
					// encoding if we weren't finishing the stream.
					reset();
					return false;

				default:
					assert(0);
					break;
			}

			++pos;
		}

		count = 0;
		pos = 0;

		return false;
	}

	uint64_t RangeEncoder::pending()
	{
		return cacheSize + 5 - 1;
	}

	uint32_t RangeEncoder::sint(RangeEncoder::Symbol s)
	{
		return static_cast<uint32_t>(s);
	}

	RangeEncoder::Symbol RangeEncoder::intToSymbol(uint32_t i)
	{
		return static_cast<RangeEncoder::Symbol>(i);
	}
}

#endif //POCKETLZMA_RANGEENCODER_HPP

/*** End of inlined file: RangeEncoder.hpp ***/


/*** Start of inlined file: LzmaMatch.hpp ***/
//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMAMATCH_HPP
#define POCKETLZMA_LZMAMATCH_HPP
namespace plz
{
	/*!
	 * A table of these is used by the LZ-based encoder to hold
	 * the length-distance pairs found by the match finder.
	 */
	class LzmaMatch
	{
		public:
			uint32_t len;
			uint32_t dist;
	};
}

#endif //POCKETLZMA_LZMAMATCH_HPP

/*** End of inlined file: LzmaMatch.hpp ***/


/*** Start of inlined file: LzmaMF.hpp ***/
//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMAMF_HPP
#define POCKETLZMA_LZMAMF_HPP

namespace plz
{
	class LzmaMF
	{
		public:
			LzmaMF() = default;

			///////////////
			// In Window //
			///////////////

			/// Pointer to buffer with data to be compressed
			uint8_t *buffer;

			/// Total size of the allocated buffer (that is, including all
			/// the extra space)
			uint32_t size;

			/// Number of bytes that must be kept available in our input history.
			/// That is, once keep_size_before bytes have been processed,
			/// buffer[read_pos - keep_size_before] is the oldest byte that
			/// must be available for reading.
			uint32_t keepSizeBefore;

			/// Number of bytes that must be kept in buffer after read_pos.
			/// That is, read_pos <= write_pos - keep_size_after as long as
			/// action is LZMA_RUN; when action != LZMA_RUN, read_pos is allowed
			/// to reach write_pos so that the last bytes get encoded too.
			uint32_t keepSizeAfter;

			/// Match finders store locations of matches using 32-bit integers.
			/// To avoid adjusting several megabytes of integers every time the
			/// input window is moved with move_window, we only adjust the
			/// offset of the buffer. Thus, buffer[value_in_hash_table - offset]
			/// is the byte pointed by value_in_hash_table.
			uint32_t offset;

			/// buffer[read_pos] is the next byte to run through the match
			/// finder. This is incremented in the match finder once the byte
			/// has been processed.
			uint32_t readPos;

			/// Number of bytes that have been ran through the match finder, but
			/// which haven't been encoded by the LZ-based encoder yet.
			uint32_t readAhead;

			/// As long as read_pos is less than read_limit, there is enough
			/// input available in buffer for at least one encoding loop.
			///
			/// Because of the stateful API, read_limit may and will get greater
			/// than read_pos quite often. This is taken into account when
			/// calculating the value for keep_size_after.
			uint32_t readLimit;

			/// buffer[write_pos] is the first byte that doesn't contain valid
			/// uncompressed data; that is, the next input byte will be copied
			/// to buffer[write_pos].
			uint32_t writePos;

			/// Number of bytes not hashed before read_pos. This is needed to
			/// restart the match finder after LZMA_SYNC_FLUSH.
			uint32_t pending;

			//////////////////
			// Match Finder //
			//////////////////

			/// Find matches. Returns the number of distance-length pairs written
			/// to the matches array. This is called only via lzma_mf_find().
			uint32_t (*find)(LzmaMF *mf, LzmaMatch *matches);

			/// Skips num bytes. This is like find() but doesn't make the
			/// distance-length pairs available, thus being a little faster.
			/// This is called only via mf_skip().
			void (*skip)(LzmaMF *mf, uint32_t num);

			uint32_t *hash;
			uint32_t *son;
			uint32_t cyclic_pos;
			uint32_t cyclic_size; // Must be dictionary size + 1.
			uint32_t hash_mask;

			/// Maximum number of loops in the match finder
			uint32_t depth;

			/// Maximum length of a match that the match finder will try to find.
			uint32_t niceLen;

			/// Maximum length of a match supported by the LZ-based encoder.
			/// If the longest match found by the match finder is nice_len,
			/// mf_find() tries to expand it up to match_len_max bytes.
			uint32_t matchLenMax;

			/// When running out of input, binary tree match finders need to know
			/// if it is due to flushing or finishing. The action is used also
			/// by the LZ-based encoders themselves.
			LzmaAction action {LzmaAction::Run};

			/// Number of elements in hash[]
			uint32_t hashCount;

			/// Number of elements in son[]
			uint32_t sonsCount;

			/*! Get the number of bytes that haven't been encoded yet (some of these
			 *  bytes may have been ran through the match finder though).
			 */
			inline uint32_t unencoded() const;

			/*!
			 *   mf_position.
			 *   Calculate the absolute offset from the beginning of the most recent
			 *   dictionary reset. Only the lowest four bits are important, so there's no
			 *   problem that we don't know the 64-bit size of the data encoded so far.
			 *
			 *   NOTE: When moving the input window, we need to do it so that the lowest
			 *   bits of dict->read_pos are not modified to keep this macro working
			 *   as intended.
			 */
			inline uint32_t position();

			/*!
			 * mf_skip.
			 *
			 * Skip the given number of bytes. This is used when a good match was found.
			 * For example, if mf_find() finds a match of 200 bytes long, the first byte
			 * of that match was already consumed by mf_find(), and the rest 199 bytes
			 * have to be skipped with mf_skip(mf, 199).
			 */
			inline void mfSkip(uint32_t amount);

			/*! lzma_mf_find / mf_find
			 * Find matches starting from the current byte
			 * \return The length of the longest match found*/
			inline uint32_t mfFind(uint32_t *count_ptr, LzmaMatch *matches);

			/*! mf_ptr.
			 * Get pointer to the first byte not ran through the match finder */
			inline const uint8_t *ptr();

			/*! mf_avail */
			inline uint32_t avail();
	};

	uint32_t LzmaMF::unencoded() const
	{
		return writePos - readPos + readAhead;
	}

	uint32_t LzmaMF::position()
	{
		return readPos - readAhead;
	}

	void LzmaMF::mfSkip(uint32_t amount)
	{
		if (amount != 0)
		{
			skip(this, amount);
			readAhead += amount;
		}
	}

	uint32_t LzmaMF::mfFind(uint32_t *count_ptr, LzmaMatch *matches)
	{
		// Call the match finder. It returns the number of length-distance
		// pairs found.
		// FIXME: Minimum count is zero, what _exactly_ is the maximum?
		const uint32_t count = find(this, matches);

		// Length of the longest match; assume that no matches were found
		// and thus the maximum length is zero.
		uint32_t len_best = 0;

		if (count > 0) {
			#ifndef NDEBUG
				// Validate the matches.
				for (uint32_t i = 0; i < count; ++i) {
					assert(matches[i].len <= niceLen);
					assert(matches[i].dist < readPos);
					assert(memcmp(ptr() - 1, ptr() - matches[i].dist - 2, matches[i].len) == 0);
				}
			#endif

			// The last used element in the array contains
			// the longest match.
			len_best = matches[count - 1].len;

			// If a match of maximum search length was found, try to
			// extend the match to maximum possible length.
			if (len_best == niceLen) {
				// The limit for the match length is either the
				// maximum match length supported by the LZ-based
				// encoder or the number of bytes left in the
				// dictionary, whichever is smaller.
				uint32_t limit = avail() + 1;
				if (limit > matchLenMax)
					limit = matchLenMax;

				// Pointer to the byte we just ran through
				// the match finder.
				const uint8_t *p1 = ptr() - 1;

				// Pointer to the beginning of the match. We need -1
				// here because the match distances are zero based.
				const uint8_t *p2 = p1 - matches[count - 1].dist - 1;

				len_best = LzmaMemcmplen(p1, p2, len_best, limit);
			}
		}

		*count_ptr = count;

		// Finally update the read position to indicate that match finder was
		// run for this dictionary offset.
		++readAhead;

		return len_best;
	}

	const uint8_t *LzmaMF::ptr()
	{
		return buffer + readPos;
	}

	uint32_t LzmaMF::avail()
	{
		return writePos - readPos;
	}
}

#endif //POCKETLZMA_LZMAMF_HPP

/*** End of inlined file: LzmaMF.hpp ***/


/*** Start of inlined file: LzmaOptions.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMAOPTIONS_HPP
#define POCKETLZMA_LZMAOPTIONS_HPP

namespace plz
{
	/*!
	 * Based on 'lzma_options_lzma'
	 * Options specific to the LZMA1 and LZMA2 filters
	 *
	 * Since LZMA1 and LZMA2 share most of the code, it's simplest to share
	 * the options structure too. For encoding, all but the reserved variables
	 * need to be initialized unless specifically mentioned otherwise.
	 * lzma_lzma_preset() can be used to get a good starting point.
	 *
	 * For raw decoding, both LZMA1 and LZMA2 need dict_size, preset_dict, and
	 * preset_dict_size (if preset_dict != NULL). LZMA1 needs also lc, lp, and pb.
	 */
	 class LzmaOptions
	 {
		 public:
			 /**
			 * \brief       Dictionary size in bytes
			 *
			 * Dictionary size indicates how many bytes of the recently processed
			 * uncompressed data is kept in memory. One method to reduce size of
			 * the uncompressed data is to store distance-length pairs, which
			 * indicate what data to repeat from the dictionary buffer. Thus,
			 * the bigger the dictionary, the better the compression ratio
			 * usually is.
			 *
			 * Maximum size of the dictionary depends on multiple things:
			 *  - Memory usage limit
			 *  - Available address space (not a problem on 64-bit systems)
			 *  - Selected match finder (encoder only)
			 *
			 * Currently the maximum dictionary size for encoding is 1.5 GiB
			 * (i.e. (1 << 30) + (1 << 29)) even on 64-bit
			 * systems for certain match finder implementation reasons. In the
			 * future, there may be match finders that support bigger
			 * dictionaries.
			 *
			 * Decoder already supports dictionaries up to 4 GiB - 1 B (i.e.
			 * UINT32_MAX), so increasing the maximum dictionary size of the
			 * encoder won't cause problems for old decoders.
			 *
			 * Because extremely small dictionaries sizes would have unneeded
			 * overhead in the decoder, the minimum dictionary size is 4096 bytes.
			 *
			 * \note        When decoding, too big dictionary does no other harm
			 *              than wasting memory.
			 */
			 uint32_t dictSize;
			 const uint32_t LZMA_DICT_SIZE_MIN {4096};
			 const uint32_t LZMA_DICT_SIZE_DEFAULT {1 << 23};

			/**
			 * \brief       Pointer to an initial dictionary
			 *
			 * It is possible to initialize the LZ77 history window using
			 * a preset dictionary. It is useful when compressing many
			 * similar, relatively small chunks of data independently from
			 * each other. The preset dictionary should contain typical
			 * strings that occur in the files being compressed. The most
			 * probable strings should be near the end of the preset dictionary.
			 *
			 * This feature should be used only in special situations. For
			 * now, it works correctly only with raw encoding and decoding.
			 * Currently none of the container formats supported by
			 * liblzma allow preset dictionary when decoding, thus if
			 * you create a .xz or .lzma file with preset dictionary, it
			 * cannot be decoded with the regular decoder functions. In the
			 * future, the .xz format will likely get support for preset
			 * dictionary though.
			 */
			const uint8_t *presetDict;

			/**
			 * \brief       Size of the preset dictionary
			 *
			 * Specifies the size of the preset dictionary. If the size is
			 * bigger than dict_size, only the last dict_size bytes are
			 * processed.
			 *
			 * This variable is read only when preset_dict is not NULL.
			 * If preset_dict is not NULL but preset_dict_size is zero,
			 * no preset dictionary is used (identical to only setting
			 * preset_dict to NULL).
			 */
			uint32_t presetDictSize;

			/**
			 * \brief       Number of literal context bits
			 *
			 * How many of the highest bits of the previous uncompressed
			 * eight-bit byte (also known as `literal') are taken into
			 * account when predicting the bits of the next literal.
			 *
			 * E.g. in typical English text, an upper-case letter is
			 * often followed by a lower-case letter, and a lower-case
			 * letter is usually followed by another lower-case letter.
			 * In the US-ASCII character set, the highest three bits are 010
			 * for upper-case letters and 011 for lower-case letters.
			 * When lc is at least 3, the literal coding can take advantage of
			 * this property in the uncompressed data.
			 *
			 * There is a limit that applies to literal context bits and literal
			 * position bits together: lc + lp <= 4. Without this limit the
			 * decoding could become very slow, which could have security related
			 * results in some cases like email servers doing virus scanning.
			 * This limit also simplifies the internal implementation in liblzma.
			 *
			 * There may be LZMA1 streams that have lc + lp > 4 (maximum possible
			 * lc would be 8). It is not possible to decode such streams with
			 * liblzma.
			 */
			uint32_t lc;
			const uint32_t LZMA_LCLP_MIN {0};
			const uint32_t LZMA_LCLP_MAX {4};
			const uint32_t LZMA_LC_DEFAULT {3};

			/**
			 * \brief       Number of literal position bits
			 *
			 * lp affects what kind of alignment in the uncompressed data is
			 * assumed when encoding literals. A literal is a single 8-bit byte.
			 * See pb below for more information about alignment.
			 */
			uint32_t lp;
			const uint32_t LZMA_LP_DEFAULT {0};

			/**
			 * \brief       Number of position bits
			 *
			 * pb affects what kind of alignment in the uncompressed data is
			 * assumed in general. The default means four-byte alignment
			 * (2^ pb =2^2=4), which is often a good choice when there's
			 * no better guess.
			 *
			 * When the alignment is known, setting pb accordingly may reduce
			 * the file size a little. E.g. with text files having one-byte
			 * alignment (US-ASCII, ISO-8859-*, UTF-8), setting pb=0 can
			 * improve compression slightly. For UTF-16 text, pb=1 is a good
			 * choice. If the alignment is an odd number like 3 bytes, pb=0
			 * might be the best choice.
			 *
			 * Even though the assumed alignment can be adjusted with pb and
			 * lp, LZMA1 and LZMA2 still slightly favor 16-byte alignment.
			 * It might be worth taking into account when designing file formats
			 * that are likely to be often compressed with LZMA1 or LZMA2.
			 */
			uint32_t pb;
			const uint32_t LZMA_PB_MIN      {0};
			const uint32_t LZMA_PB_MAX      {4};
			const uint32_t LZMA_PB_DEFAULT  {2};

			/** Compression mode */
			Mode mode {Mode::Fast};

			/**
			 * \brief       Nice length of a match
			 *
			 * This determines how many bytes the encoder compares from the match
			 * candidates when looking for the best match. Once a match of at
			 * least nice_len bytes long is found, the encoder stops looking for
			 * better candidates and encodes the match. (Naturally, if the found
			 * match is actually longer than nice_len, the actual length is
			 * encoded; it's not truncated to nice_len.)
			 *
			 * Bigger values usually increase the compression ratio and
			 * compression time. For most files, 32 to 128 is a good value,
			 * which gives very good compression ratio at good speed.
			 *
			 * The exact minimum value depends on the match finder. The maximum
			 * is 273, which is the maximum length of a match that LZMA1 and
			 * LZMA2 can encode.
			 */
			uint32_t niceLen;

			/** Match finder ID */
			MatchFinder mf { MatchFinder::HC3 };

			/**
			 * \brief       Maximum search depth in the match finder
			 *
			 * For every input byte, match finder searches through the hash chain
			 * or binary tree in a loop, each iteration going one step deeper in
			 * the chain or tree. The searching stops if
			 *  - a match of at least nice_len bytes long is found;
			 *  - all match candidates from the hash chain or binary tree have
			 *    been checked; or
			 *  - maximum search depth is reached.
			 *
			 * Maximum search depth is needed to prevent the match finder from
			 * wasting too much time in case there are lots of short match
			 * candidates. On the other hand, stopping the search before all
			 * candidates have been checked can reduce compression ratio.
			 *
			 * Setting depth to zero tells liblzma to use an automatic default
			 * value, that depends on the selected match finder and nice_len.
			 * The default is in the range [4, 200] or so (it may vary between
			 * liblzma versions).
			 *
			 * Using a bigger depth value than the default can increase
			 * compression ratio in some cases. There is no strict maximum value,
			 * but high values (thousands or millions) should be used with care:
			 * the encoder could remain fast enough with typical input, but
			 * malicious input could cause the match finder to slow down
			 * dramatically, possibly creating a denial of service attack.
			 */
			uint32_t depth;

			inline bool isLclppbValid() const;
	 };

	bool LzmaOptions::isLclppbValid() const
	{
		if(lc <= LZMA_LCLP_MAX && lp <= LZMA_LCLP_MAX
		   && lc + lp <= LZMA_LCLP_MAX
		   && pb <= LZMA_PB_MAX)
			return true;

		return false;
		//return StatusCode::ErrorInLclppbCheckOnLzmaOptions;
	}
}

#endif //POCKETLZMA_LZMAOPTIONS_HPP

/*** End of inlined file: LzmaOptions.hpp ***/


/*** Start of inlined file: LzmaOptimal.hpp ***/
//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMAOPTIMAL_HPP
#define POCKETLZMA_LZMAOPTIMAL_HPP

namespace plz
{
	class LzmaOptimal
	{
		public:
			LzmaOptimal() = default;
			LzmaState state;

			bool prev1IsLiteral;
			bool prev2;

			uint32_t posPrev2;
			uint32_t backPrev2;

			uint32_t price;
			uint32_t posPrev;  // pos_next;
			uint32_t backPrev;

			uint32_t backs[REPS];
	};
}

#endif //POCKETLZMA_LZMAOPTIMAL_HPP

/*** End of inlined file: LzmaOptimal.hpp ***/


/*** Start of inlined file: LengthEncoder.hpp ***/
//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LENGTHENCODER_HPP
#define POCKETLZMA_LENGTHENCODER_HPP

namespace plz
{
	class LengthEncoder
	{
		public:
			LengthEncoder() = default;

			probability choice;
			probability choice2;
			probability low[POS_STATES_MAX][LEN_LOW_SYMBOLS];
			probability mid[POS_STATES_MAX][LEN_MID_SYMBOLS];
			probability high[LEN_HIGH_SYMBOLS];

			uint32_t prices[POS_STATES_MAX][LEN_SYMBOLS];
			uint32_t tableSize;
			uint32_t counters[POS_STATES_MAX];

			/*! length_encoder_reset */
			inline void reset(const uint32_t num_pos_states, const bool fast_mode);

		private:
			/*! length_update_prices */
			inline void updatePrices(const uint32_t pos_state);
	};

	void LengthEncoder::reset(const uint32_t num_pos_states, const bool fast_mode)
	{
		BitReset(choice);
		BitReset(choice2);

		for (size_t pos_state = 0; pos_state < num_pos_states; ++pos_state) {
			BittreeReset(low[pos_state], LEN_LOW_BITS);
			BittreeReset(mid[pos_state], LEN_MID_BITS);
		}

		BittreeReset(high, LEN_HIGH_BITS);

		if (!fast_mode)
			for (uint32_t pos_state = 0; pos_state < num_pos_states;
				 ++pos_state)
				updatePrices(pos_state);

		return;
	}

	void LengthEncoder::updatePrices(const uint32_t pos_state)
	{
		const uint32_t table_size = table_size;
		counters[pos_state] = table_size;

		const uint32_t a0 = Price::RcBit0Price(choice);
		const uint32_t a1 = Price::RcBit1Price(choice);
		const uint32_t b0 = a1 + Price::RcBit0Price(choice2);
		const uint32_t b1 = a1 + Price::RcBit1Price(choice2);
		uint32_t *const p = prices[pos_state];

		uint32_t i;
		for (i = 0; i < table_size && i < LEN_LOW_SYMBOLS; ++i)
			p[i] = a0 + Price::RcBittreePrice(low[pos_state],
											  LEN_LOW_BITS, i);

		for (; i < table_size && i < LEN_LOW_SYMBOLS + LEN_MID_SYMBOLS; ++i)
			p[i] = b0 + Price::RcBittreePrice(mid[pos_state],
											  LEN_MID_BITS, i - LEN_LOW_SYMBOLS);

		for (; i < table_size; ++i)
			p[i] = b1 + Price::RcBittreePrice(high, LEN_HIGH_BITS,
											  i - LEN_LOW_SYMBOLS - LEN_MID_SYMBOLS);

	}
}

#endif //POCKETLZMA_LENGTHENCODER_HPP

/*** End of inlined file: LengthEncoder.hpp ***/


/*** Start of inlined file: Lzma1Encoder.hpp ***/
//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMA1ENCODER_HPP
#define POCKETLZMA_LZMA1ENCODER_HPP

namespace plz
{
	class Lzma1Encoder
	{
		public:
			Lzma1Encoder() = default;

			RangeEncoder rc;

			/// State
			LzmaState state;

			/// The four most recent match distances
			uint32_t reps[REPS];

			/// Array of match candidates
			LzmaMatch matches[MATCH_LEN_MAX + 1];

			/// Number of match candidates in matches[]
			uint32_t matchesCount;

			/// Variable to hold the length of the longest match between calls
			/// to lzma_lzma_optimum_*().
			uint32_t longestMatchLength;

			/// True if using getoptimumfast
			bool fastMode;

			/// True if the encoder has been initialized by encoding the first
			/// byte as a literal.
			bool isInitialized;

			/// True if the range encoder has been flushed, but not all bytes
			/// have been written to the output buffer yet.
			bool isFlushed;

			uint32_t posMask;         ///< (1 << pos_bits) - 1
			uint32_t literalContextBits;
			uint32_t literalPosMask;

			// These are the same as in lzma_decoder.c. See comments there.
			probability literal[LITERAL_CODERS_MAX][LITERAL_CODER_SIZE];
			probability isMatch[STATES][POS_STATES_MAX];
			probability isRep[STATES];
			probability isRep0[STATES];
			probability isRep1[STATES];
			probability isRep2[STATES];
			probability isRep0Long[STATES][POS_STATES_MAX];
			probability distSlot[DIST_STATES][DIST_SLOTS];
			probability distSpecial[FULL_DISTANCES - DIST_MODEL_END];
			probability distAlign[ALIGN_SIZE];

			// These are the same as in lzma_decoder.c except that the encoders
			// include also price tables.
			LengthEncoder matchLenEncoder;
			LengthEncoder repLenEncoder;

			// Price tables
			uint32_t distSlotPrices[DIST_STATES][DIST_SLOTS];
			uint32_t distPrices[DIST_STATES][FULL_DISTANCES];
			uint32_t distTableSize;
			uint32_t matchPriceCount;

			uint32_t alignPrices[ALIGN_SIZE];
			uint32_t alignPriceCount;

			// Optimal
			uint32_t optsEndIndex;
			uint32_t optsCurrentIndex;
			LzmaOptimal opts[OPTS];

			/*! encode_init */
			inline bool init(LzmaMF *mf);

			/*! lzma_lzma_optimum_normal */
			inline void lzmaOptimumNormal(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position);

			/*! lzma_lzma_optimum_fast */
			inline void lzmaOptimumFast(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res);

		private:
			/*! fill_dist_prices */
			inline void fillDistPrices();

			/*! fill_align_prices */
			inline void fillAlignPrices();

			/*! helper1
			 *  A horrible function doing something important.
			 * */
			inline uint32_t helper1(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position);

			/*! helper2
			 *  Another horrible function doing something important.*/
			inline uint32_t helper2(uint32_t *reps, const uint8_t *buf,
					uint32_t len_end, uint32_t position, const uint32_t cur,
					const uint32_t nice_len, const uint32_t buf_avail_full);

			/*! get_literal_price */
			inline uint32_t getLiteralPrice(const uint32_t pos, const uint32_t prev_byte, const bool match_mode,
							  uint32_t match_byte, uint32_t symbol);

	};

	bool Lzma1Encoder::init(LzmaMF *mf)
	{
		assert(mf->position() == 0);

		if (mf->readPos == mf->readLimit)
		{
			if (mf->action == LzmaAction::Run)
				return false; // We cannot do anything.

			// We are finishing (we cannot get here when flushing).
			assert(mf->writePos == mf->readPos);
			assert(mf->action == LzmaAction::Finish);
		}
		else
		{
			// Do the actual initialization. The first LZMA symbol must
			// always be a literal.
			mf->mfSkip(1);
			mf->readAhead = 0;
			rc.bit(&isMatch[0][0], 0);
			rc.bittree(literal[0], 8, mf->buffer[0]);
		}

		// Initialization is done (except if empty file).
		isInitialized = true;

		return true;
	}

	void Lzma1Encoder::lzmaOptimumFast(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res)
	{
		const uint32_t nice_len = mf->niceLen;

		uint32_t len_main;
		uint32_t matches_count;
		if (mf->readAhead == 0) {
			len_main = mf->mfFind(&matches_count, matches);
		} else {
			assert(mf->readAhead == 1);
			len_main = longestMatchLength;
			matches_count = matches_count;
		}

		const uint8_t *buf = mf->ptr() - 1;
		const uint32_t buf_avail = my_min(mf->avail() + 1, MATCH_LEN_MAX);

		if (buf_avail < 2) {
			// There's not enough input left to encode a match.
			*back_res = UINT32_MAX;
			*len_res = 1;
			return;
		}

		// Look for repeated matches; scan the previous four match distances
		uint32_t rep_len = 0;
		uint32_t rep_index = 0;

		for (uint32_t i = 0; i < REPS; ++i)
		{
			// Pointer to the beginning of the match candidate
			const uint8_t *const buf_back = buf - reps[i] - 1;

			// If the first two bytes (2 == MATCH_LEN_MIN) do not match,
			// this rep is not useful.
			if (not_equal_16(buf, buf_back))
				continue;

			// The first two bytes matched.
			// Calculate the length of the match.
			const uint32_t len = LzmaMemcmplen(
					buf, buf_back, 2, buf_avail);

			// If we have found a repeated match that is at least
			// nice_len long, return it immediately.
			if (len >= nice_len) {
				*back_res = i;
				*len_res = len;
				mf->mfSkip(len - 1);
				return;
			}

			if (len > rep_len) {
				rep_index = i;
				rep_len = len;
			}
		}

		// We didn't find a long enough repeated match. Encode it as a normal
		// match if the match length is at least nice_len.
		if (len_main >= nice_len) {
			*back_res = matches[matches_count - 1].dist + REPS;
			*len_res = len_main;
			mf->mfSkip(len_main - 1);
			return;
		}

		uint32_t back_main = 0;
		if (len_main >= 2) {
			back_main = matches[matches_count - 1].dist;

			while (matches_count > 1 && len_main ==
										matches[matches_count - 2].len + 1) {
				if (!change_pair(matches[
										 matches_count - 2].dist,
								 back_main))
					break;

				--matches_count;
				len_main = matches[matches_count - 1].len;
				back_main = matches[matches_count - 1].dist;
			}

			if (len_main == 2 && back_main >= 0x80)
				len_main = 1;
		}

		if (rep_len >= 2)
		{
			if (rep_len + 1 >= len_main ||
			   (rep_len + 2 >= len_main && back_main > (1 << 9)) ||
			   (rep_len + 3 >= len_main && back_main > (1 << 15)))
			{
				*back_res = rep_index;
				*len_res = rep_len;
				mf->mfSkip(rep_len - 1);
				return;
			}
		}

		if (len_main < 2 || buf_avail <= 2) {
			*back_res = UINT32_MAX;
			*len_res = 1;
			return;
		}

		// Get the matches for the next byte. If we find a better match,
		// the current byte is encoded as a literal.
		longestMatchLength = mf->mfFind(&matches_count, matches);

		if (longestMatchLength >= 2) {
			const uint32_t new_dist = matches[
					matches_count - 1].dist;

			if ((longestMatchLength >= len_main
				 && new_dist < back_main)
				|| (longestMatchLength == len_main + 1
					&& !change_pair(back_main, new_dist))
				|| (longestMatchLength > len_main + 1)
				|| (longestMatchLength + 1 >= len_main
					&& len_main >= 3
					&& change_pair(new_dist, back_main))) {
				*back_res = UINT32_MAX;
				*len_res = 1;
				return;
			}
		}

		// In contrast to LZMA SDK, dictionary could not have been moved
		// between mf_find() calls, thus it is safe to just increment
		// the old buf pointer instead of recalculating it with mf_ptr().
		++buf;

		const uint32_t limit = my_max(2, len_main - 1);

		for (uint32_t i = 0; i < REPS; ++i) {
			if (memcmp(buf, buf - reps[i] - 1, limit) == 0) {
				*back_res = UINT32_MAX;
				*len_res = 1;
				return;
			}
		}

		*back_res = back_main + REPS;
		*len_res = len_main;
		mf->mfSkip(len_main - 2);
	}

	void Lzma1Encoder::lzmaOptimumNormal(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position)
	{
		// If we have symbols pending, return the next pending symbol.
		if (optsEndIndex != optsCurrentIndex)
		{
			assert(mf->readAhead > 0);
			*len_res = opts[optsCurrentIndex].posPrev
					   - optsCurrentIndex;
			*back_res = opts[optsCurrentIndex].backPrev;
			optsCurrentIndex = opts[
					optsCurrentIndex].posPrev;
			return;
		}

		// Update the price tables. In LZMA SDK <= 4.60 (and possibly later)
		// this was done in both initialization function and in the main loop.
		// In liblzma they were moved into this single place.
		if (mf->readAhead == 0)
		{
			if (matchPriceCount >= (1 << 7))
				fillDistPrices();

			if (alignPriceCount >= ALIGN_SIZE)
				fillAlignPrices();
		}

		// TODO: This needs quite a bit of cleaning still. But splitting
		// the original function into two pieces makes it at least a little
		// more readable, since those two parts don't share many variables.

		uint32_t len_end = helper1(mf, back_res, len_res, position);
		if (len_end == UINT32_MAX)
			return;

		uint32_t reps[REPS];
		memcpy(reps, reps, sizeof(reps));

		uint32_t cur;
		for (cur = 1; cur < len_end; ++cur) {
			assert(cur < OPTS);

			longestMatchLength = mf->mfFind(&matchesCount, matches);

			if (longestMatchLength >= mf->niceLen)
				break;

			len_end = helper2(coder, reps, mf_ptr(mf) - 1, len_end,
							  position + cur, cur, mf->nice_len,
							  my_min(mf_avail(mf) + 1, OPTS - 1 - cur));
		}

		backward(coder, len_res, back_res, cur);
	}

	void Lzma1Encoder::fillDistPrices()
	{
		for (uint32_t dist_state = 0; dist_state < DIST_STATES; ++dist_state)
		{

			uint32_t *const dist_slot_prices
					= distSlotPrices[dist_state];

			// Price to encode the dist_slot.
			for (uint32_t dist_slot = 0; dist_slot < distTableSize; ++dist_slot)
			{
				dist_slot_prices[dist_slot] = Price::RcBittreePrice(distSlot[dist_state], DIST_SLOT_BITS, dist_slot);
			}

			// For matches with distance >= FULL_DISTANCES, add the price
			// of the direct bits part of the match distance. (Align bits
			// are handled by fill_align_prices()).
			for (uint32_t dist_slot = DIST_MODEL_END; dist_slot < distTableSize; ++dist_slot)
				dist_slot_prices[dist_slot] += Price::RcDirectPrice(((dist_slot >> 1) - 1) - ALIGN_BITS);

			// Distances in the range [0, 3] are fully encoded with
			// dist_slot, so they are used for dist_prices
			// as is.
			for (uint32_t i = 0; i < DIST_MODEL_START; ++i)
				distPrices[dist_state][i] = dist_slot_prices[i];
		}

		// Distances in the range [4, 127] depend on dist_slot and
		// dist_special. We do this in a loop separate from the above
		// loop to avoid redundant calls to get_dist_slot().
		for (uint32_t i = DIST_MODEL_START; i < FULL_DISTANCES; ++i)
		{
			const uint32_t dist_slot = GetDistSlot(i);
			const uint32_t footer_bits = ((dist_slot >> 1) - 1);
			const uint32_t base = (2 | (dist_slot & 1)) << footer_bits;
			const uint32_t price = Price::RcBittreeReversePrice(distSpecial + base - dist_slot - 1, footer_bits, i - base);

			for (uint32_t dist_state = 0; dist_state < DIST_STATES;
				 ++dist_state)
			{
				distPrices[dist_state][i] = price + distSlotPrices[dist_state][dist_slot];
			}
		}

		matchPriceCount = 0;
	}

	void Lzma1Encoder::fillAlignPrices()
	{
		for (uint32_t i = 0; i < ALIGN_SIZE; ++i)
		{
			alignPrices[i] = Price::RcBittreeReversePrice(distAlign, ALIGN_BITS, i);
		}

		alignPriceCount = 0;
	}

	uint32_t Lzma1Encoder::helper1(LzmaMF *mf, uint32_t *back_res, uint32_t *len_res, uint32_t position)
	{
		const uint32_t nice_len = mf->niceLen;

		uint32_t len_main;
		uint32_t matches_count;

		if (mf->readAhead == 0) {
			len_main = mf->mfFind(&matches_count, matches);
		} else {
			assert(mf->readAhead == 1);
			len_main = longestMatchLength;
			matches_count = matches_count;
		}

		const uint32_t buf_avail = my_min(mf->avail() + 1, MATCH_LEN_MAX);
		if (buf_avail < 2) {
			*back_res = UINT32_MAX;
			*len_res = 1;
			return UINT32_MAX;
		}

		const uint8_t *const buf = mf->ptr() - 1;

		uint32_t rep_lens[REPS];
		uint32_t rep_max_index = 0;

		for (uint32_t i = 0; i < REPS; ++i) {
			const uint8_t *const buf_back = buf - reps[i] - 1;

			if (not_equal_16(buf, buf_back)) {
				rep_lens[i] = 0;
				continue;
			}

			rep_lens[i] = LzmaMemcmplen(buf, buf_back, 2, buf_avail);

			if (rep_lens[i] > rep_lens[rep_max_index])
				rep_max_index = i;
		}

		if (rep_lens[rep_max_index] >= nice_len) {
			*back_res = rep_max_index;
			*len_res = rep_lens[rep_max_index];
			mf->mfSkip(*len_res - 1);
			return UINT32_MAX;
		}

		if (len_main >= nice_len) {
			*back_res = matches[matches_count - 1].dist + REPS;
			*len_res = len_main;
			mf->mfSkip(len_main - 1);
			return UINT32_MAX;
		}

		const uint8_t current_byte = *buf;
		const uint8_t match_byte = *(buf - reps[0] - 1);

		if (len_main < 2 && current_byte != match_byte
			&& rep_lens[rep_max_index] < 2) {
			*back_res = UINT32_MAX;
			*len_res = 1;
			return UINT32_MAX;
		}

		opts[0].state = state;

		const uint32_t pos_state = position & posMask;

		opts[1].price = Price::RcBit0Price(isMatch[state][pos_state])
							   + getLiteralPrice(position, buf[-1], !is_literal_state(state), match_byte, current_byte);

		make_literal(&opts[1]);

		const uint32_t match_price = rc_bit_1_price(
				is_match[state][pos_state]);
		const uint32_t rep_match_price = match_price
										 + rc_bit_1_price(is_rep[state]);

		if (match_byte == current_byte) {
			const uint32_t short_rep_price = rep_match_price
											 + get_short_rep_price(
					coder, state, pos_state);

			if (short_rep_price < opts[1].price) {
				opts[1].price = short_rep_price;
				make_short_rep(&opts[1]);
			}
		}

		const uint32_t len_end = my_max(len_main, rep_lens[rep_max_index]);

		if (len_end < 2) {
			*back_res = opts[1].backPrev;
			*len_res = 1;
			return UINT32_MAX;
		}

		opts[1].posPrev = 0;

		for (uint32_t i = 0; i < REPS; ++i)
			opts[0].backs[i] = reps[i];

		uint32_t len = len_end;
		do {
			opts[len].price = RC_INFINITY_PRICE;
		} while (--len >= 2);

		for (uint32_t i = 0; i < REPS; ++i) {
			uint32_t rep_len = rep_lens[i];
			if (rep_len < 2)
				continue;

			const uint32_t price = rep_match_price + get_pure_rep_price(
					coder, i, state, pos_state);

			do {
				const uint32_t cur_and_len_price = price
												   + get_len_price(
						&rep_len_encoder,
						rep_len, pos_state);

				if (cur_and_len_price < opts[rep_len].price) {
					opts[rep_len].price = cur_and_len_price;
					opts[rep_len].pos_prev = 0;
					opts[rep_len].back_prev = i;
					opts[rep_len].prev_1_is_literal = false;
				}
			} while (--rep_len >= 2);
		}

		const uint32_t normal_match_price = match_price
											+ rc_bit_0_price(is_rep[state]);

		len = rep_lens[0] >= 2 ? rep_lens[0] + 1 : 2;
		if (len <= len_main) {
			uint32_t i = 0;
			while (len > matches[i].len)
				++i;

			for(; ; ++len) {
				const uint32_t dist = matches[i].dist;
				const uint32_t cur_and_len_price = normal_match_price
												   + get_dist_len_price(coder,
																		dist, len, pos_state);

				if (cur_and_len_price < opts[len].price) {
					opts[len].price = cur_and_len_price;
					opts[len].pos_prev = 0;
					opts[len].back_prev = dist + REPS;
					opts[len].prev_1_is_literal = false;
				}

				if (len == matches[i].len)
					if (++i == matches_count)
						break;
			}
		}

		return len_end;
	}

	uint32_t Lzma1Encoder::getLiteralPrice(const uint32_t pos, const uint32_t prev_byte, const bool match_mode, uint32_t match_byte, uint32_t symbol)
	{
		const probability *const subcoder = literal_subcoder(literal, literalContextBits, literalPosMask, pos, prev_byte);

		uint32_t price = 0;

		if (!match_mode) {
			price = Price::RcBittreePrice(subcoder, 8, symbol);
		} else {
			uint32_t offset = 0x100;
			symbol += 1 << 8;

			do {
				match_byte <<= 1;

				const uint32_t match_bit = match_byte & offset;
				const uint32_t subcoder_index
						= offset + match_bit + (symbol >> 8);
				const uint32_t bit = (symbol >> 7) & 1;
				price += Price::RcBitPrice(subcoder[subcoder_index], bit);

				symbol <<= 1;
				offset &= ~(match_byte ^ symbol);

			} while (symbol < (1 << 16));
		}

		return price;
	}

	uint32_t Lzma1Encoder::helper2(uint32_t *reps, const uint8_t *buf, uint32_t len_end, uint32_t position, const uint32_t cur, const uint32_t nice_len,
						  const uint32_t buf_avail_full)
	{
		uint32_t matches_count = matches_count;
		uint32_t new_len = longestMatchLength;
		uint32_t pos_prev = opts[cur].posPrev;
		LzmaState lzmaState;

		if (opts[cur].prev1IsLiteral) {
			--pos_prev;

			if (opts[cur].prev2) {
				lzmaState = opts[opts[cur].posPrev2].state;

				if (opts[cur].backPrev2 < REPS)
					UpdateLongRep(lzmaState);
				else
					UpdateMatch(lzmaState);

			} else {
				lzmaState = opts[pos_prev].state;
			}

			update_literal(lzmaState);

		} else {
			lzmaState = opts[pos_prev].state;
		}

		if (pos_prev == cur - 1) {
			if (is_short_rep(opts[cur]))
				UpdateShortRep(lzmaState);
			else
				update_literal(lzmaState);
		} else {
			uint32_t pos;
			if (opts[cur].prev1IsLiteral
				&& opts[cur].prev2)
			{
				pos_prev = opts[cur].posPrev2;
				pos = opts[cur].backPrev2;
				UpdateLongRep(lzmaState);
			} else {
				pos = opts[cur].backPrev;
				if (pos < REPS)
					UpdateLongRep(lzmaState);
				else
					UpdateMatch(lzmaState);
			}

			if (pos < REPS)
			{
				reps[0] = opts[pos_prev].backs[pos];

				uint32_t i;
				for (i = 1; i <= pos; ++i)
					reps[i] = opts[pos_prev].backs[i - 1];

				for (; i < REPS; ++i)
					reps[i] = opts[pos_prev].backs[i];

			}
			else
			{
				reps[0] = pos - REPS;

				for (uint32_t i = 1; i < REPS; ++i)
					reps[i] = opts[pos_prev].backs[i - 1];
			}
		}

		opts[cur].state = lzmaState;

		for (uint32_t i = 0; i < REPS; ++i)
			opts[cur].backs[i] = reps[i];

		const uint32_t cur_price = opts[cur].price;

		const uint8_t current_byte = *buf;
		const uint8_t match_byte = *(buf - reps[0] - 1);

		const uint32_t pos_state = position & posMask;

		const uint32_t cur_and_1_price = cur_price
										 + Price::RcBit0Price(isMatch[(uint8_t)lzmaState][pos_state])
										 + getLiteralPrice(position, buf[-1],
															 !is_literal_state(lzmaState), match_byte, current_byte);

		bool next_is_literal = false;

		if (cur_and_1_price < opts[cur + 1].price) {
			opts[cur + 1].price = cur_and_1_price;
			opts[cur + 1].pos_prev = cur;
			make_literal(&opts[cur + 1]);
			next_is_literal = true;
		}

		const uint32_t match_price = cur_price
									 + rc_bit_1_price(is_match[lzmaState][pos_state]);
		const uint32_t rep_match_price = match_price
										 + rc_bit_1_price(is_rep[lzmaState]);

		if (match_byte == current_byte
			&& !(opts[cur + 1].pos_prev < cur
				 && opts[cur + 1].back_prev == 0)) {

			const uint32_t short_rep_price = rep_match_price
											 + get_short_rep_price(coder, lzmaState, pos_state);

			if (short_rep_price <= opts[cur + 1].price) {
				opts[cur + 1].price = short_rep_price;
				opts[cur + 1].pos_prev = cur;
				make_short_rep(&opts[cur + 1]);
				next_is_literal = true;
			}
		}

		if (buf_avail_full < 2)
			return len_end;

		const uint32_t buf_avail = my_min(buf_avail_full, nice_len);

		if (!next_is_literal && match_byte != current_byte) { // speed optimization
			// try literal + rep0
			const uint8_t *const buf_back = buf - reps[0] - 1;
			const uint32_t limit = my_min(buf_avail_full, nice_len + 1);

			const uint32_t len_test = lzma_memcmplen(buf, buf_back, 1, limit) - 1;

			if (len_test >= 2) {
				lzma_lzma_state state_2 = lzmaState;
				update_literal(state_2);

				const uint32_t pos_state_next = (position + 1) & pos_mask;
				const uint32_t next_rep_match_price = cur_and_1_price
													  + rc_bit_1_price(is_match[state_2][pos_state_next])
													  + rc_bit_1_price(is_rep[state_2]);

				//for (; len_test >= 2; --len_test) {
				const uint32_t offset = cur + 1 + len_test;

				while (len_end < offset)
					opts[++len_end].price = RC_INFINITY_PRICE;

				const uint32_t cur_and_len_price = next_rep_match_price
												   + get_rep_price(coder, 0, len_test,
																   state_2, pos_state_next);

				if (cur_and_len_price < opts[offset].price) {
					opts[offset].price = cur_and_len_price;
					opts[offset].pos_prev = cur + 1;
					opts[offset].back_prev = 0;
					opts[offset].prev_1_is_literal = true;
					opts[offset].prev_2 = false;
				}
				//}
			}
		}

		uint32_t start_len = 2; // speed optimization

		for (uint32_t rep_index = 0; rep_index < REPS; ++rep_index) {
			const uint8_t *const buf_back = buf - reps[rep_index] - 1;
			if (not_equal_16(buf, buf_back))
				continue;

			uint32_t len_test = lzma_memcmplen(buf, buf_back, 2, buf_avail);

			while (len_end < cur + len_test)
				opts[++len_end].price = RC_INFINITY_PRICE;

			const uint32_t len_test_temp = len_test;
			const uint32_t price = rep_match_price + get_pure_rep_price(
					coder, rep_index, lzmaState, pos_state);

			do {
				const uint32_t cur_and_len_price = price
												   + get_len_price(&rep_len_encoder,
																   len_test, pos_state);

				if (cur_and_len_price < opts[cur + len_test].price) {
					opts[cur + len_test].price = cur_and_len_price;
					opts[cur + len_test].pos_prev = cur;
					opts[cur + len_test].back_prev = rep_index;
					opts[cur + len_test].prev_1_is_literal = false;
				}
			} while (--len_test >= 2);

			len_test = len_test_temp;

			if (rep_index == 0)
				start_len = len_test + 1;

			uint32_t len_test_2 = len_test + 1;
			const uint32_t limit = my_min(buf_avail_full,
										  len_test_2 + nice_len);
			// NOTE: len_test_2 may be greater than limit so the call to
			// lzma_memcmplen() must be done conditionally.
			if (len_test_2 < limit)
				len_test_2 = lzma_memcmplen(buf, buf_back, len_test_2, limit);

			len_test_2 -= len_test + 1;

			if (len_test_2 >= 2) {
				lzma_lzma_state state_2 = lzmaState;
				update_long_rep(state_2);

				uint32_t pos_state_next = (position + len_test) & pos_mask;

				const uint32_t cur_and_len_literal_price = price
														   + get_len_price(&rep_len_encoder,
																		   len_test, pos_state)
														   + rc_bit_0_price(is_match[state_2][pos_state_next])
														   + get_literal_price(coder, position + len_test,
																			   buf[len_test - 1], true,
																			   buf_back[len_test], buf[len_test]);

				update_literal(state_2);

				pos_state_next = (position + len_test + 1) & pos_mask;

				const uint32_t next_rep_match_price = cur_and_len_literal_price
													  + rc_bit_1_price(is_match[state_2][pos_state_next])
													  + rc_bit_1_price(is_rep[state_2]);

				//for(; len_test_2 >= 2; len_test_2--) {
				const uint32_t offset = cur + len_test + 1 + len_test_2;

				while (len_end < offset)
					opts[++len_end].price = RC_INFINITY_PRICE;

				const uint32_t cur_and_len_price = next_rep_match_price
												   + get_rep_price(coder, 0, len_test_2,
																   state_2, pos_state_next);

				if (cur_and_len_price < opts[offset].price) {
					opts[offset].price = cur_and_len_price;
					opts[offset].pos_prev = cur + len_test + 1;
					opts[offset].back_prev = 0;
					opts[offset].prev_1_is_literal = true;
					opts[offset].prev_2 = true;
					opts[offset].pos_prev_2 = cur;
					opts[offset].back_prev_2 = rep_index;
				}
				//}
			}
		}

		//for (uint32_t len_test = 2; len_test <= new_len; ++len_test)
		if (new_len > buf_avail) {
			new_len = buf_avail;

			matches_count = 0;
			while (new_len > matches[matches_count].len)
				++matches_count;

			matches[matches_count++].len = new_len;
		}

		if (new_len >= start_len) {
			const uint32_t normal_match_price = match_price
												+ rc_bit_0_price(is_rep[lzmaState]);

			while (len_end < cur + new_len)
				opts[++len_end].price = RC_INFINITY_PRICE;

			uint32_t i = 0;
			while (start_len > matches[i].len)
				++i;

			for (uint32_t len_test = start_len; ; ++len_test) {
				const uint32_t cur_back = matches[i].dist;
				uint32_t cur_and_len_price = normal_match_price
											 + get_dist_len_price(coder,
																  cur_back, len_test, pos_state);

				if (cur_and_len_price < opts[cur + len_test].price) {
					opts[cur + len_test].price = cur_and_len_price;
					opts[cur + len_test].pos_prev = cur;
					opts[cur + len_test].back_prev
							= cur_back + REPS;
					opts[cur + len_test].prev_1_is_literal = false;
				}

				if (len_test == matches[i].len) {
					// Try Match + Literal + Rep0
					const uint8_t *const buf_back = buf - cur_back - 1;
					uint32_t len_test_2 = len_test + 1;
					const uint32_t limit = my_min(buf_avail_full,
												  len_test_2 + nice_len);

					// NOTE: len_test_2 may be greater than limit
					// so the call to lzma_memcmplen() must be
					// done conditionally.
					if (len_test_2 < limit)
						len_test_2 = lzma_memcmplen(buf, buf_back,
													len_test_2, limit);

					len_test_2 -= len_test + 1;

					if (len_test_2 >= 2) {
						lzma_lzma_state state_2 = lzmaState;
						update_match(state_2);
						uint32_t pos_state_next
								= (position + len_test) & pos_mask;

						const uint32_t cur_and_len_literal_price = cur_and_len_price
																   + rc_bit_0_price(
								is_match[state_2][pos_state_next])
																   + get_literal_price(coder,
																					   position + len_test,
																					   buf[len_test - 1],
																					   true,
																					   buf_back[len_test],
																					   buf[len_test]);

						update_literal(state_2);
						pos_state_next = (pos_state_next + 1) & pos_mask;

						const uint32_t next_rep_match_price
								= cur_and_len_literal_price
								  + rc_bit_1_price(
										is_match[state_2][pos_state_next])
								  + rc_bit_1_price(is_rep[state_2]);

						// for(; len_test_2 >= 2; --len_test_2) {
						const uint32_t offset = cur + len_test + 1 + len_test_2;

						while (len_end < offset)
							opts[++len_end].price = RC_INFINITY_PRICE;

						cur_and_len_price = next_rep_match_price
											+ get_rep_price(coder, 0, len_test_2,
															state_2, pos_state_next);

						if (cur_and_len_price < opts[offset].price) {
							opts[offset].price = cur_and_len_price;
							opts[offset].pos_prev = cur + len_test + 1;
							opts[offset].back_prev = 0;
							opts[offset].prev_1_is_literal = true;
							opts[offset].prev_2 = true;
							opts[offset].pos_prev_2 = cur;
							opts[offset].back_prev_2
									= cur_back + REPS;
						}
						//}
					}

					if (++i == matches_count)
						break;
				}
			}
		}

		return len_end;
	}
}

#endif //POCKETLZMA_LZMA1ENCODER_HPP

/*** End of inlined file: Lzma1Encoder.hpp ***/


/*** Start of inlined file: Lzma2Coder.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA2CODER_HPP
#define POCKETLZMA_LZMA2CODER_HPP

namespace plz
{
	class Lzma2Coder
	{
		public:
			Lzma2Coder() = default;

			enum class Sequence : uint8_t
			{
				Init = 0,
				Encode = 1,
				Copy = 2,
				UncompressedHeader = 3,
				UncompressedCopy = 4
			};

			Sequence sequence {Sequence::Init};
			/*! LZMA encoder */
			void *lzma;

			/*! LZMA options currently in use. */
			LzmaOptions optCur;

			bool needProperties;
			bool needStateReset;
			bool needDictionaryReset;

			/*! Uncompressed size of a chunk */
			size_t uncompressedSize;

			/*!
			 * Compressed size of a chunk (excluding headers); this is also used
			 * to indicate the end of buf[] in SEQ_LZMA_COPY.
			 */
			size_t compressedSize;

			/*! Read position in buf[] */
			size_t bufPos;

			/*! Buffer to hold the chunk header and LZMA compressed data */
			uint8_t buf[LZMA2_MAX_HEADER_SIZE + LZMA2_MAX_CHUNK_SIZE];
	};
}
#endif //POCKETLZMA_LZMA2CODER_HPP

/*** End of inlined file: Lzma2Coder.hpp ***/


/*** Start of inlined file: ICompressor.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_ICOMPRESSOR_HPP
#define POCKETLZMA_ICOMPRESSOR_HPP

namespace plz
{
	class ICompressor
	{
		public:
			virtual plz::StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) = 0;
	};
}

#endif //POCKETLZMA_ICOMPRESSOR_HPP

/*** End of inlined file: ICompressor.hpp ***/


/*** Start of inlined file: IDecompressor.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_IDECOMPRESSOR_HPP
#define POCKETLZMA_IDECOMPRESSOR_HPP

namespace plz
{
	class IDecompressor
	{
		public:
			virtual plz::StatusCode decompress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) = 0;
	};
}

#endif //POCKETLZMA_IDECOMPRESSOR_HPP

/*** End of inlined file: IDecompressor.hpp ***/


/*** Start of inlined file: LzmaCompressor.hpp ***/
//
// Created by robin on 24.12.2020.
//

#ifndef POCKETLZMA_LZMACOMPRESSOR_HPP
#define POCKETLZMA_LZMACOMPRESSOR_HPP

namespace plz
{
	class LzmaCompressor : public ICompressor
	{
		public:
			LzmaCompressor() = default;
			inline StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) override;

		protected:
			inline StatusCode encode(Lzma1Encoder *coder, LzmaMF *mf,
									 uint8_t *out, size_t *out_pos,
									 size_t out_size);
			inline StatusCode lzmaEncode(Lzma1Encoder * coder, LzmaMF * mf,
							 uint8_t * out, size_t * out_pos,
							 size_t out_size, uint32_t limit);

			/*! Encodes lc/lp/pb into one byte. */
			inline StatusCode lzmaLclppbEncode(const LzmaOptions *options, uint8_t *byte);
			inline StatusCode lzmaEncoderReset(Lzma1Encoder *coder, const LzmaOptions *options);
			inline bool isOptionsValid(const LzmaOptions *options);
			inline StatusCode literalInit(probability (*probs)[LITERAL_CODER_SIZE], uint32_t lc, uint32_t lp);
	};

	StatusCode LzmaCompressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
	{

		return StatusCode::UndefinedError;
	}

	StatusCode LzmaCompressor::encode(Lzma1Encoder *coder, LzmaMF *mf, uint8_t *out, size_t *out_pos, size_t out_size)
	{
		// Plain LZMA has no support for sync-flushing.
		if (unlikely(mf->action == LzmaAction::SyncFlush))
			return StatusCode::OptionsError;
		return lzmaEncode(coder, mf, out, out_pos, out_size, UINT32_MAX);
	}

	StatusCode LzmaCompressor::lzmaEncode(Lzma1Encoder *coder, LzmaMF *mf, uint8_t *out, size_t *out_pos, size_t out_size, uint32_t limit)
	{
		// Initialize the stream if no data has been encoded yet.
		if (!coder->isInitialized && !coder->init(mf))
			return StatusCode::Ok;

		// Get the lowest bits of the uncompressed offset from the LZ layer.
		uint32_t position = mf->position();

		while (true) {
			// Encode pending bits, if any. Calling this before encoding
			// the next symbol is needed only with plain LZMA, since
			// LZMA2 always provides big enough buffer to flush
			// everything out from the range encoder. For the same reason,
			// rc_encode() never returns true when this function is used
			// as part of LZMA2 encoder.
			if (coder->rc.encode(out, out_pos, out_size))
			{
				assert(limit == UINT32_MAX);
				return StatusCode::Ok;
			}

			// With LZMA2 we need to take care that compressed size of
			// a chunk doesn't get too big.
			// FIXME? Check if this could be improved.
			if (limit != UINT32_MAX
				&& (mf->readPos - mf->readAhead >= limit ||
					*out_pos + coder->rc.pending() >= LZMA2_MAX_CHUNK_SIZE - LOOP_INPUT_MAX))
			{
				break;
			}

			// Check that there is some input to process.
			if (mf->readPos >= mf->readLimit)
			{
				if (mf->action == LzmaAction::Run)
					return StatusCode::Ok;

				if (mf->readAhead == 0)
					break;
			}

			// Get optimal match (repeat position and length).
			// Value ranges for pos:
			//   - [0, REPS): repeated match
			//   - [REPS, UINT32_MAX):
			//     match at (pos - REPS)
			//   - UINT32_MAX: not a match but a literal
			// Value ranges for len:
			//   - [MATCH_LEN_MIN, MATCH_LEN_MAX]
			uint32_t len;
			uint32_t back;

			if (coder->fastMode)
				lzma_lzma_optimum_fast(coder, mf, &back, &len);
			else
				lzma_lzma_optimum_normal(
						coder, mf, &back, &len, position);

			encode_symbol(coder, mf, back, len, position);

			position += len;
		}

		if (!coder->is_flushed) {
			coder->is_flushed = true;

			// We don't support encoding plain LZMA streams without EOPM,
			// and LZMA2 doesn't use EOPM at LZMA level.
			if (limit == UINT32_MAX)
				encode_eopm(coder, position);

			// Flush the remaining bytes from the range encoder.
			rc_flush(&coder->rc);

			// Copy the remaining bytes to the output buffer. If there
			// isn't enough output space, we will copy out the remaining
			// bytes on the next call to this function by using
			// the rc_encode() call in the encoding loop above.
			if (rc_encode(&coder->rc, out, out_pos, out_size)) {
				assert(limit == UINT32_MAX);
				return LZMA_OK;
			}
		}

		// Make it ready for the next LZMA2 chunk.
		coder->is_flushed = false;

		return StatusCode::StreamEnd;
	}

	StatusCode LzmaCompressor::lzmaLclppbEncode(const LzmaOptions *options, uint8_t *byte)
	{
		StatusCode lclppbStatus = options->isLclppbValid() ? StatusCode::Ok : StatusCode::ErrorInLclppbCheckOnLzmaOptions;
		if (lclppbStatus != StatusCode::Ok)
			return lclppbStatus;

		*byte = (options->pb * 5 + options->lp) * 9 + options->lc;
		if(*byte > (4 * 5 + 4) * 9 + 8)
			return StatusCode::InvalidLclppbByteValue;

		return StatusCode::Ok;
	}

	StatusCode LzmaCompressor::lzmaEncoderReset(Lzma1Encoder *coder, const LzmaOptions *options)
	{
		if (!isOptionsValid(options))
			return StatusCode::OptionsError;

		coder->posMask = (1U << options->pb) - 1;
		coder->literalContextBits = options->lc;
		coder->literalPosMask = (1U << options->lp) - 1;

		// Range coder
		coder->rc.reset();

		// State
		coder->state = LzmaState::LitLit;
		for (size_t i = 0; i < REPS; ++i)
			coder->reps[i] = 0;

		StatusCode initStatus = literalInit(coder->literal, options->lc, options->lp);
		if(initStatus != StatusCode::Ok)
			return initStatus;

		// Bit encoders
		for (size_t i = 0; i < STATES; ++i) {
			for (size_t j = 0; j <= coder->posMask; ++j) {
				BitReset(coder->isMatch[i][j]);
				BitReset(coder->isRep0Long[i][j]);
			}

			BitReset(coder->isRep[i]);
			BitReset(coder->isRep0[i]);
			BitReset(coder->isRep1[i]);
			BitReset(coder->isRep2[i]);
		}

		for (size_t i = 0; i < FULL_DISTANCES - DIST_MODEL_END; ++i)
			BitReset(coder->distSpecial[i]);

		// Bit tree encoders
		for (size_t i = 0; i < DIST_STATES; ++i)
			BittreeReset(coder->distSlot[i], DIST_SLOT_BITS);

		BittreeReset(coder->distAlign, ALIGN_BITS);

		// Length encoders
		coder->matchLenEncoder.reset(1U << options->pb, coder->fastMode);
		coder->repLenEncoder.reset(1U << options->pb, coder->fastMode);
		// length_encoder_reset(&coder->matchLenEncoder,
		//                      1U << options->pb, coder->fastMode);
//
		// length_encoder_reset(&coder->repLenEncoder,
		//                      1U << options->pb, coder->fastMode);

		// Price counts are incremented every time appropriate probabilities
		// are changed. price counts are set to zero when the price tables
		// are updated, which is done when the appropriate price counts have
		// big enough value, and lzma_mf.read_ahead == 0 which happens at
		// least every OPTS (a few thousand) possible price count increments.
		//
		// By resetting price counts to UINT32_MAX / 2, we make sure that the
		// price tables will be initialized before they will be used (since
		// the value is definitely big enough), and that it is OK to increment
		// price counts without risk of integer overflow (since UINT32_MAX / 2
		// is small enough). The current code doesn't increment price counts
		// before initializing price tables, but it maybe done in future if
		// we add support for saving the state between LZMA2 chunks.
		coder->match_price_count = UINT32_MAX / 2;
		coder->align_price_count = UINT32_MAX / 2;

		coder->opts_end_index = 0;
		coder->opts_current_index = 0;
		return StatusCode::Ok;
	}

	bool LzmaCompressor::isOptionsValid(const LzmaOptions *options)
	{
		// Validate some of the options. LZ encoder validates nice_len too
		// but we need a valid value here earlier.
		return options->isLclppbValid()
			   && options->niceLen >= MATCH_LEN_MIN
			   && options->niceLen <= MATCH_LEN_MAX
			   && (options->mode == Mode::Fast
				   || options->mode == Mode::Normal);
	}

	StatusCode LzmaCompressor::literalInit(probability (*probs)[LITERAL_CODER_SIZE], uint32_t lc, uint32_t lp)
	{
		if(lc + lp > LZMA_LCLP_MAX)
			return StatusCode::LzmaEncodingLiteralInitError;

		const uint32_t coders = 1U << (lc + lp);

		for (uint32_t i = 0; i < coders; ++i)
			for (uint32_t j = 0; j < LITERAL_CODER_SIZE; ++j)
				BitReset(probs[i][j]);

		return StatusCode::Ok;
	}

}
#endif //POCKETLZMA_LZMACOMPRESSOR_HPP

/*** End of inlined file: LzmaCompressor.hpp ***/


/*** Start of inlined file: Lzma2Compressor.hpp ***/
//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA2COMPRESSOR_HPP
#define POCKETLZMA_LZMA2COMPRESSOR_HPP

namespace plz
{
	class Lzma2Compressor : public LzmaCompressor
	{
		public:
			Lzma2Compressor() = default;

			inline StatusCode compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output) override;

		private:
			inline StatusCode encode(Lzma2Coder *coder, LzmaMF * mf, uint8_t * out, size_t * out_pos,
									 size_t out_size);
			/*!
			 * Validates the coder then assigns header data.
			 * Based on lzma2_header_lzma
			 * @param coder
			 * @return
			 */
			inline StatusCode validateCoderAndAssignHeader(Lzma2Coder *coder);
			/*! Based on lzma2_header_uncompressed */
			inline StatusCode headerUncompressed(Lzma2Coder *coder);

			uint32_t m_bufferSize {LZMA2_MAX_UNCOMPRESSED_SIZE};
			uint32_t m_chunkSize {LZMA2_MAX_CHUNK_SIZE};
	};

	StatusCode Lzma2Compressor::validateCoderAndAssignHeader(Lzma2Coder *coder)
	{
		if(coder->uncompressedSize == 0) return StatusCode::UncompressedSizeIsZero;
		if(coder->uncompressedSize > LZMA2_MAX_UNCOMPRESSED_SIZE) return StatusCode::UncompressedSizeTooLargeLzma2;
		if(coder->compressedSize == 0) return StatusCode::CompressedSizeIsZero;
		if(coder->compressedSize > LZMA2_MAX_CHUNK_SIZE) return StatusCode::CompressedSizeIsLargerThanMaxChunkLzma2;

		size_t pos;

		if (coder->needProperties) {
			pos = 0;

			if (coder->needDictionaryReset)
				coder->buf[pos] = 0x80 + (3 << 5);
			else
				coder->buf[pos] = 0x80 + (2 << 5);
		} else {
			pos = 1;

			if (coder->needStateReset)
				coder->buf[pos] = 0x80 + (1 << 5);
			else
				coder->buf[pos] = 0x80;
		}

		// Set the start position for copying.
		coder->bufPos = pos;

		// Uncompressed size
		size_t size = coder->uncompressedSize - 1;
		coder->buf[pos++] += size >> 16;
		coder->buf[pos++] = (size >> 8) & 0xFF;
		coder->buf[pos++] = size & 0xFF;

		// Compressed size
		size = coder->compressedSize - 1;
		coder->buf[pos++] = size >> 8;
		coder->buf[pos++] = size & 0xFF;

		// Properties, if needed
		if (coder->needProperties)
		{
			StatusCode status = lzmaLclppbEncode(&coder->optCur, coder->buf + pos);
			if(status != StatusCode::Ok)
				return status;
		}

		coder->needProperties = false;
		coder->needStateReset = false;
		coder->needDictionaryReset = false;

		// The copying code uses coder->compressed_size to indicate the end
		// of coder->buf[], so we need add the maximum size of the header here.
		coder->compressedSize += LZMA2_MAX_HEADER_SIZE;

		return StatusCode::Ok;
	}

	StatusCode Lzma2Compressor::encode(Lzma2Coder *coder, LzmaMF *mf, uint8_t *out, size_t *out_pos, size_t out_size)
	{
		while (*out_pos < out_size)
		{
			switch (coder->sequence)
			{
				case Lzma2Coder::Sequence::Init:
					// If there's no input left and we are flushing or finishing,
					// don't start a new chunk.
					if (mf->unencoded() == 0) //(MfUnencoded(mf) == 0)
					{
						// Write end of payload marker if finishing.
						if (mf->action == LzmaAction::Finish)
							out[(*out_pos)++] = 0;

						return (mf->action == LzmaAction::Run) ? StatusCode::Ok : StatusCode::StreamEnd;
					}

					if (coder->needStateReset)
						return_if_error(lzma_lzma_encoder_reset(
								coder->lzma, &coder->optCur));

					coder->uncompressedSize = 0;
					coder->compressedSize = 0;
					coder->sequence = Lzma2Coder::Sequence::Encode;

					// Fall through

				case Lzma2Coder::Sequence::Encode:
				{
					// Calculate how much more uncompressed data this chunk
					// could accept.
					const uint32_t left = LZMA2_MAX_UNCOMPRESSED_SIZE
										  - coder->uncompressedSize;
					uint32_t limit;

					if (left < mf->matchLenMax)
					{
						// Must flush immediately since the next LZMA symbol
						// could make the uncompressed size of the chunk too
						// big.
						limit = 0;
					}
					else
					{
						// Calculate maximum read_limit that is OK from point
						// of view of LZMA2 chunk size.
						limit = mf->readPos - mf->readAhead
								+ left - mf->matchLenMax;
					}

					// Save the start position so that we can update
					// coder->uncompressed_size.
					const uint32_t read_start = mf->readPos - mf->readAhead;

					// Call the LZMA encoder until the chunk is finished.
					const StatusCode ret = lzma_lzma_encode(coder->lzma, mf,
														  coder->buf + LZMA2_MAX_HEADER_SIZE,
														  &coder->compressedSize,
														  LZMA2_MAX_CHUNK_SIZE, limit);

					coder->uncompressedSize += mf->readPos - mf->readAhead
											   - read_start;

					assert(coder->compressedSize <= LZMA2_MAX_CHUNK_SIZE);
					assert(coder->uncompressedSize <= LZMA2_MAX_UNCOMPRESSED_SIZE);

					if (ret != StatusCode::StreamEnd)
						return StatusCode::Ok;

					// See if the chunk compressed. If it didn't, we encode it
					// as uncompressed chunk. This saves a few bytes of space
					// and makes decoding faster.
					if (coder->compressedSize >= coder->uncompressedSize)
					{
						coder->uncompressedSize += mf->readAhead;
						assert(coder->uncompressedSize
							   <= LZMA2_MAX_UNCOMPRESSED_SIZE);
						mf->readAhead = 0;
						headerUncompressed(coder);
						coder->needStateReset = true;
						coder->sequence = Lzma2Coder::Sequence::UncompressedHeader;
						break;
					}

					// The chunk did compress at least by one byte, so we store
					// the chunk as LZMA.
					lzma2_header_lzma(coder);

					coder->sequence = Lzma2Coder::Sequence::Copy;
				}

					// Fall through

				case Lzma2Coder::Sequence::Copy:
					// Copy the compressed chunk along its headers to the
					// output buffer.
					lzma_bufcpy(coder->buf, &coder->bufPos,
								coder->compressedSize,
								out, out_pos, out_size);
					if (coder->bufPos != coder->compressedSize)
						return StatusCode::Ok;

					coder->sequence = Lzma2Coder::Sequence::Init;
					break;

				case Lzma2Coder::Sequence::UncompressedHeader:
					// Copy the three-byte header to indicate uncompressed chunk.
					lzma_bufcpy(coder->buf, &coder->bufPos,
								LZMA2_HEADER_SIZE_UNCOMPRESSED,
								out, out_pos, out_size);
					if (coder->bufPos != LZMA2_HEADER_SIZE_UNCOMPRESSED)
						return StatusCode::Ok;

					coder->sequence = Lzma2Coder::Sequence::UncompressedCopy;

					// Fall through

				case Lzma2Coder::Sequence::UncompressedCopy:
					// Copy the uncompressed data as is from the dictionary
					// to the output buffer.
					mf_read(mf, out, out_pos, out_size, &coder->uncompressedSize);
					if (coder->uncompressedSize != 0)
						return StatusCode::Ok;

					coder->sequence = Lzma2Coder::Sequence::Init;
					break;
			}
		}

		return StatusCode::Ok;
	}

	StatusCode Lzma2Compressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
	{

		return StatusCode::UndefinedError;
	}

	StatusCode Lzma2Compressor::headerUncompressed(Lzma2Coder *coder)
	{
		if (coder->uncompressedSize == 0) return StatusCode::UncompressedSizeIsZero;
		if (coder->uncompressedSize > LZMA2_MAX_CHUNK_SIZE) return StatusCode::UncompressedSizeTooLargeLzma2;

		// If this is the first chunk, we need to include dictionary
		// reset indicator.
		if (coder->needDictionaryReset)
			coder->buf[0] = 1;
		else
			coder->buf[0] = 2;

		coder->needDictionaryReset = false;

		// "Compressed" size
		coder->buf[1] = (coder->uncompressedSize - 1) >> 8;
		coder->buf[2] = (coder->uncompressedSize - 1) & 0xFF;

		// Set the start position for copying.
		coder->bufPos = 0;

		return StatusCode::Ok;
	}

}

#endif //POCKETLZMA_LZMA2COMPRESSOR_HPP

/*** End of inlined file: Lzma2Compressor.hpp ***/

#endif //POCKETLZMA_POCKETLZMA_H

