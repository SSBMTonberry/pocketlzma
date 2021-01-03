# PocketLzma
PocketLzma is a cross-platform singleheader `LZMA` compression/decompression library for C++11. To use it, all you need is one
`pocketlzma.hpp` file, and you are good to go! The library is able to read data from both `files` and `memory`!

PocketLzma is designed to be a mix of a modern yet portable C++ library, only utilizing C++11 to make sure it can be used in projects where the latest versions of C++ are not available or otherwise not allowed to use.

# What is LZMA?
LZMA stands for Lempel–Ziv–Markov chain Algorithm, and is an algorithm used for lossless data compression.
The algorithm has been developed by **Igor Pavlov** since the late 90s. The C implementation made by Igor Pavlov is in fact what provides the core functionalty required for PocketLzma to work (with a few modifications to make it cross-platform and single-header compatible).

## Documentation

There is a `Doxygen` generated documentation of PocketLzma that can be found [HERE](https://ssbmtonberry.github.io/pocketlzma/html/)

# How to use PocketLzma
PocketLzma is first of all designed to be easy to use! If you want to see a fully working code example, take a look at the `pocketlzma_program.cpp` file in the root folder of this project. Otherwise: Follow along for some short examples.

#### First an important note! Since PocketLzma internally uses Igor Pavlov's C code, you are required to `#define POCKETLZMA_LZMA_C_DEFINE` ONCE (and only once) before including `pocketlzma.hpp`. The reason for this is to make sure the implementations of the C-code are only included once. In other words: If you include the `pocketlzma.hpp` file several places in your project, the other places must not use the `#define`   

## The File API
PocketLzma has a very simple API for file communication. However, you are free to use something else
if you want. PocketLzma doesn't care how you got your data.
Example:

```c++
std::string path = "./yourFile.txt";

//When you are 100% sure your loading will never fail, you can use this
std::vector<uint8_t> data1 = plz::File::FromFile(path);

//However - I recommend to use this overload instead:
std::vector<uint8_t> data2;
plz::FileStatus fileStatus = plz::File::FromFile(path, data2);
//If something went wrong
if(fileStatus.status() != plz::FileStatus::Code::Ok)
{
    plz::FileStatus::Code statusCode = fileStatus.status(); //PocketLzma status code. Will be useful if errors not causing exceptions happen.
    
    //You may or may not have some error information here
    int code = fileStatus.code();                       //Code returned from the OS in cases where an exception is thrown
    std::string msg = fileStatus.message();             //Message from the OS in cases where an exception is thrown
    std::string exception = fileStatus.exception();     //Exception message from the OS in cases where an exception is thrown
    std::string category = fileStatus.category();       //Error category defined bythe OS in cases where an exception is thrown
}

//You can use memory data directly in PocketLzma, but you can use this if you want to transform them into a byte vector.
std::vector<uint8_t> memoryData;
plz::File::FromMemory(memfiles::_JSON_TEST_OK_HEADER_LZMA, memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE, memoryData);

//Finally, you can write to files like this
std::string writePath = "./yourOutputFile.txt";
plz::FileStatus fileWriteStatus = plz::File::ToFile(writePath, data2);

```

## Compression
```c++
#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

int main()
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::vector<uint8_t> data;
    std::vector<uint8_t> compressedData;
    plz::FileStatus fileStatus = plz::File::FromFile(path, data);
    if(fileStatus.status() == plz::FileStatus::Code::Ok)
    {
        plz::PocketLzma p;
        /*!
         *  Possibilities:
         *  Default
         *  Fastest
         *  Fast
         *  GoodCompression
         *  BestCompression
         */
        p.usePreset(plz::Preset::GoodCompression); //Default is used when preset is not set.
        plz::StatusCode status = p.compress(data, compressedData);
        if(status == plz::StatusCode::Ok)
        {
            std::string outputPath = "./../../content/to_compress/to/j.lzma";
            plz::FileStatus writeStatus = plz::File::ToFile(outputPath, compressedData);
            if(writeStatus.status() == plz::FileStatus::Code::Ok)
            {
                //Process completed successfully!
            }
        }
    }
    return 0;
}
```

## Compression (Advanced)
If you are familiar with LZMA, you can use your own compression settings rather than using presets. Making it possible to tune every parameter to get your own balance of speed and compression ratio.

```c++
#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

int main()
{
    std::string path = "./../../content/to_compress/from/json_test.json";
    std::vector<uint8_t> data;
    std::vector<uint8_t> compressedData;
    plz::FileStatus fileStatus = plz::File::FromFile(path, data);
    if(fileStatus.status() == plz::FileStatus::Code::Ok)
    {
        plz::Settings settings;
        // These are actual default values used when choosing the Default preset,
        // but shows the parameters that can be tuned
        settings.level                = 5;
        settings.dictionarySize       = 1 << 24;
        settings.literalContextBits   = 3;
        settings.literalPositionBits  = 0;
        settings.positionBits         = 2;
        settings.fastBytes            = 32;

        plz::PocketLzma p {settings}; //You can alternatively use: p.setSettings(settings);
        plz::StatusCode status = p.compress(data, compressedData);
        if(status == plz::StatusCode::Ok)
        {
            std::string outputPath = "./../../content/to_compress/to/j.lzma";
            plz::FileStatus writeStatus = plz::File::ToFile(outputPath, compressedData);
            if(writeStatus.status() == plz::FileStatus::Code::Ok)
            {
                //Process completed successfully!
            }
        }
    }
    return 0;
}
```

## Decompression
```c++
#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

int main()
{
    std::string path = "./../../content/to_compress/to/j.lzma";
    std::vector<uint8_t> data;
    std::vector<uint8_t> decompressedData;
    plz::FileStatus fileStatus = plz::File::FromFile(path, data);
    if(fileStatus.status() == plz::FileStatus::Code::Ok)
    {
        //No settings / presets are used during decompression!
        plz::PocketLzma p;
        plz::StatusCode status = p.decompress(data, decompressedData);
        if(status == plz::StatusCode::Ok)
        {
            std::string outputPath = "./../../content/to_compress/from/j.json";
            plz::FileStatus writeStatus = plz::File::ToFile(outputPath, decompressedData);
            if(writeStatus.status() == plz::FileStatus::Code::Ok)
            {
                //Process completed successfully!
            }
        }
    }
    return 0;
}
```

## Decompression using data from memory
You can use data directly from memory (both for compression and decompression), if you please. If you need a program to generate in-memory files, you can use my [f2src](https://github.com/SSBMTonberry/f2src/) program to do that job for you.

```c++
plz::PocketLzma p;

//Alternative 1
std::vector<uint8_t> decompressed;
plz::StatusCode status = p.decompress(memfiles::_JSON_TEST_OK_HEADER_LZMA, 
                                      memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE, 
                                      decompressed);

...

//Alternative 2
std::vector<uint8_t> bytes = plz::File::FromMemory(memfiles::_JSON_TEST_OK_HEADER_LZMA, 
                                                   memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE);
status = p.decompress(bytes, decompressed);

...

//Alternative 3
std::vector<uint8_t> bytes;
plz::File::FromMemory(memfiles::_JSON_TEST_OK_HEADER_LZMA, 
                      memfiles::_JSON_TEST_OK_HEADER_LZMA_SIZE, 
                      bytes);
status = p.decompress(bytes, decompressed);
```

# Benchmarks

### **Specs:**
- **CPU**: Intel Core i7-6700 @ 8x 4GHz
- **GPU**: GeForce GTX 1080
- **RAM**: 15969MiB
- **OS**: Linux (Manjaro 20.2 Nibia) - **Kernel**: x86_64 Linux 4.14.209-1-MANJARO

## .json compression benchmark (20 runs)
| Preset          | Size before | Size after | Average time      | Min. time    |Max. time     |
| :-----:         |:----------: | :---------:| :---------------: |:------------:|:------------:|
| Fastest         |70230 bytes  | 3364 bytes |  2.25587 ms       | 1.99016 ms   | 3.17568 ms   |
| Fast            |70230 bytes  | 3283 bytes |  2.29653 ms       | 2.02276 ms   | 3.54542 ms   |
| Default         |70230 bytes  | 2693 bytes |  20.7517 ms       | 19.3443 ms   | 25.8784 ms   |
| GoodCompression |70230 bytes  | 2485 bytes |  30.6193 ms       | 28.5697 ms   | 38.3485 ms   |
| BestCompression |70230 bytes  | 2451 bytes |  45.8248 ms       | 43.4147 ms   | 57.934  ms   |

## .json decompression benchmark (20 runs)
| Preset (when compressed)| Size before | Size after | Average time      | Min. time    |Max. time     |
| :-----:                 |:----------: | :---------:| :---------------: |:------------:|:------------:|
| Fastest                 |3364 bytes   | 70230 bytes|  0.502977 ms      | 0.497585 ms  | 0.536352 ms  |
| Fast                    |3283 bytes   | 70230 bytes|  0.492965 ms      | 0.488002 ms  | 0.50396 ms   |
| Default                 |2693 bytes   | 70230 bytes|  0.42965 ms       | 0.423914 ms  | 0.448674 ms  |
| GoodCompression         |2485 bytes   | 70230 bytes|  0.405111 ms      | 0.402765 ms  | 0.419498 ms  |
| BestCompression         |2451 bytes   | 70230 bytes|  0.401403 ms      | 0.400253 ms  | 0.406537 ms  |

## .slp (binary file) compression benchmark (5 runs)
| Preset          | Size before | Size after | Average time      | Min. time    |Max. time     |
| :-----:         |:----------: | :---------:| :---------------: |:------------:|:------------:|
| Fastest         |4145823 bytes|702789 bytes|  253.102 ms       | 232.259 ms   | 330.66 ms    |
| Fast            |4145823 bytes|677754 bytes|  355.459 ms       | 321.867 ms   | 455.138 ms   |
| Default         |4145823 bytes|572742 bytes|  1889.8  ms       | 1852.6 ms    | 1993.94 ms   |
| GoodCompression |4145823 bytes|521168 bytes|  2888.82 ms       | 2879.99 ms   | 2915.07 ms   |
| BestCompression |4145823 bytes|520358 bytes|  3140.49 ms       | 3107.47 ms   | 3168.36 ms   |

## .slp (binary file) decompression benchmark (5 runs)
| Preset (when compressed)| Size before | Size after  | Average time      | Min. time    |Max. time     |
| :-----:                 |:----------: | :----------:| :---------------: |:------------:|:------------:|
| Fastest                 |702789 bytes |4145823 bytes|  85.5723 ms       | 84.871 ms    | 87.2708 ms   |
| Fast                    |677754 bytes |4145823 bytes|  81.7042 ms       | 81.4525 ms   | 82.052 ms    |
| Default                 |572742 bytes |4145823 bytes|  78.0439 ms       | 77.7491 ms   | 78.7667 ms   |
| GoodCompression         |521168 bytes |4145823 bytes|  74.8225 ms       | 74.1976 ms   | 76.2949 ms   |
| BestCompression         |520358 bytes |4145823 bytes|  74.3353 ms       | 74.1313 ms   | 74.5187 ms   |

# Credits
All credits goes to **Igor Pavlov**, the genius behind the LZMA compression algorithm. He has distributed all his work under Public Domain for anyone to use. PocketLzma uses parts of Igor Pavlov's LZMA related C code in [LZMA SDK v19.00](https://www.7-zip.org/sdk.html).

While PocketLzma goes under the still very permissive `BSD-2-Clause License` I've created an optional cross-platform amalgamated `lzma_c.hpp`, which contains a slightly altered version of Igor Pavlov's LZMA implementation for C, and is released under the same Public Domain license to honor Igor's work. This can be found in the `extras` folder, but keep in mind that this file is not supported in any way and may not co-exist with PocketLzma. 
