//
// Created by robin on 19.12.2020.
//

#include "pocketlzma.h"
#include <memory>
int main()
{
    std::unique_ptr<plz::ICompressor> compressor;
    std::vector<uint8_t> input {34, 32, 156, 4, 4, 4, 9};
    std::vector<uint8_t> output;
    compressor->compress(input, output);

    return 0;
}