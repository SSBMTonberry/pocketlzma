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
            inline uint32_t mfUnencoded() const;
    };

    uint32_t LzmaMF::mfUnencoded() const
    {
        return writePos - readPos + readAhead;
    }
}

#endif //POCKETLZMA_LZMAMF_HPP
