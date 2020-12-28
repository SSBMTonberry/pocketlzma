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
