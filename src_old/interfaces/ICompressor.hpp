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
