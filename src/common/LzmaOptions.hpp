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
