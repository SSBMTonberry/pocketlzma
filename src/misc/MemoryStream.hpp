//
// Created by robin on 28.12.2020.
//

#ifndef POCKETLZMA_MEMORYSTREAM_HPP
#define POCKETLZMA_MEMORYSTREAM_HPP

#include "MemoryBuffer.hpp"

namespace plz
{
    class MemoryStream : public std::istream {
        public:
            MemoryStream(const uint8_t *p, size_t l) :
                    std::istream(&m_buffer),
                    m_buffer(p, l)
            {
                m_size = l;
                rdbuf(&m_buffer);
            }

            size_t size() const { return m_size; }

        private:
            MemoryBuffer m_buffer;
            size_t m_size;
    };
}

#endif //POCKETLZMA_MEMORYSTREAM_HPP
