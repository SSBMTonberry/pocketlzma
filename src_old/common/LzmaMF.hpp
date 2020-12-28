//
// Created by robin on 25.12.2020.
//

#ifndef POCKETLZMA_LZMAMF_HPP
#define POCKETLZMA_LZMAMF_HPP

#include "LzOptions.hpp"

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

            std::unique_ptr<uint32_t> hash;
            std::unique_ptr<uint32_t> son;
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

            /*! mf_read */
            inline void read(uint8_t *out, size_t *out_pos, size_t out_size, size_t *left);

            /*! lz_encoder_init */
            inline bool init(const LzOptions *lz_options);

            #error implement lz_encoder_prepare!
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

    void LzmaMF::read(uint8_t *out, size_t *out_pos, size_t out_size, size_t *left)
    {
        const size_t out_avail = out_size - *out_pos;
        const size_t copy_size = my_min(out_avail, *left);

        assert(readAhead == 0);
        assert(readPos >= *left);

        memcpy(out + *out_pos, buffer + readPos - *left,
               copy_size);

        *out_pos += copy_size;
        *left -= copy_size;
    }

    bool LzmaMF::init(const LzOptions *lz_options)
    {
        // Allocate the history buffer.
        // RBP: Might need to uncomment and do something here
        //if (buffer == nullptr) {
        //    // lzma_memcmplen() is used for the dictionary buffer
        //    // so we need to allocate a few extra bytes to prevent
        //    // it from reading past the end of the buffer.
        //    buffer = lzma_alloc(size + LZMA_MEMCMPLEN_EXTRA,
        //                            allocator);
        //    if (buffer == NULL)
        //        return true;
        //    // Keep Valgrind happy with lzma_memcmplen() and initialize
        //    // the extra bytes whose value may get read but which will
        //    // effectively get ignored.
        //    memzero(buffer + size, LZMA_MEMCMPLEN_EXTRA);
        //}

        // Use cyclic_size as initial offset. This allows
        // avoiding a few branches in the match finders. The downside is
        // that match finder needs to be normalized more often, which may
        // hurt performance with huge dictionaries.
        offset = cyclic_size;
        readPos = 0;
        readAhead = 0;
        readLimit = 0;
        writePos = 0;
        pending = 0;

        #if UINT32_MAX >= SIZE_MAX / 4
                // Check for integer overflow. (Huge dictionaries are not
            // possible on 32-bit CPU.)
            if (hash_count > SIZE_MAX / sizeof(uint32_t)
                    || sons_count > SIZE_MAX / sizeof(uint32_t))
                return true;
        #endif

        // Allocate and initialize the hash table. Since EMPTY_HASH_VALUE
        // is zero, we can use lzma_alloc_zero() or memzero() for hash.
        //
        // We don't need to initialize son, but not doing that may
        // make Valgrind complain in normalization (see normalize() in
        // lz_encoder_mf.c). Skipping the initialization is *very* good
        // when big dictionary is used but only small amount of data gets
        // actually compressed: most of the son won't get actually
        // allocated by the kernel, so we avoid wasting RAM and improve
        // initialization speed a lot.
        if (hash == nullptr)
        {
            hash = std::unique_ptr<uint32_t>(new uint32_t (hashCount * sizeof(uint32_t)));
            son = std::unique_ptr<uint32_t>(new uint32_t (sonsCount * sizeof(uint32_t)));
        }
        else
        {
/*
		for (uint32_t i = 0; i < hash_count; ++i)
			hash[i] = EMPTY_HASH_VALUE;
*/
            //memzero(hash, hashCount * sizeof(uint32_t));
            memset(hash.get(), 0, hashCount * sizeof(uint32_t));
        }

        cyclic_pos = 0;

        // Handle preset dictionary.
        if (lz_options->presetDict != nullptr
            && lz_options->presetDictSize > 0) {
            // If the preset dictionary is bigger than the actual
            // dictionary, use only the tail.
            writePos = my_min(lz_options->presetDictSize, size);
            memcpy(buffer, lz_options->presetDict
                               + lz_options->presetDictSize - writePos,
                   writePos);
            action = LzmaAction::SyncFlush;
            mfSkip(writePos);
        }

        action = LzmaAction::Run;

        return false;
    }
}

#endif //POCKETLZMA_LZMAMF_HPP
