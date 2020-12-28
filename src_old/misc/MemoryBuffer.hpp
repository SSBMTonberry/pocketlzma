//
// Created by robin on 28.12.2020.
//

#ifndef POCKETLZMA_MEMORYBUFFER_HPP
#define POCKETLZMA_MEMORYBUFFER_HPP

#include <iostream>

namespace plz
{
    class MemoryBuffer : public std::basic_streambuf<char> {
        public:
            MemoryBuffer(const uint8_t *p, size_t l) {
                setg((char*)p, (char*)p, (char*)p + l);
            }
    };
}

#endif //POCKETLZMA_MEMORYBUFFER_HPP
