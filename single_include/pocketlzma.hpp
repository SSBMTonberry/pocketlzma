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
}
#endif //POCKETLZMA_POCKETLZMA_COMMON_H

/*** End of inlined file: pocketlzma_common.h ***/


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
	};
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
	 };
}

#endif //POCKETLZMA_LZMAOPTIONS_HPP

/*** End of inlined file: LzmaOptions.hpp ***/


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


/*** Start of inlined file: CommonFunctions.hpp ***/
//
// Created by robin on 24.12.2020.
//

#ifndef POCKETLZMA_COMMONFUNCTIONS_HPP
#define POCKETLZMA_COMMONFUNCTIONS_HPP

namespace plz
{
	static inline StatusCode IsLclppbValid(const LzmaOptions *options)
	{
		if(options->lc <= LZMA_LCLP_MAX && options->lp <= LZMA_LCLP_MAX
		   && options->lc + options->lp <= LZMA_LCLP_MAX
		   && options->pb <= LZMA_PB_MAX)
			return StatusCode::Ok;

		return StatusCode::ErrorInLclppbCheckOnLzmaOptions;
	}
}

#endif //POCKETLZMA_COMMONFUNCTIONS_HPP

/*** End of inlined file: CommonFunctions.hpp ***/


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
			/*! Encodes lc/lp/pb into one byte. */
			inline StatusCode lzmaLclppbEncode(const LzmaOptions *options, uint8_t *byte);
	};

	StatusCode LzmaCompressor::compress(const std::vector<uint8_t> &input, std::vector<uint8_t> &output)
	{
		return StatusCode::UndefinedError;
	}

	StatusCode LzmaCompressor::lzmaLclppbEncode(const LzmaOptions *options, uint8_t *byte)
	{
		StatusCode lclppbStatus = IsLclppbValid(options);
		if (lclppbStatus != StatusCode::Ok)
			return lclppbStatus;

		*byte = (options->pb * 5 + options->lp) * 9 + options->lc;
		if(*byte > (4 * 5 + 4) * 9 + 8)
			return StatusCode::InvalidLclppbByteValue;

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
					if (mf_unencoded(mf) == 0)
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

