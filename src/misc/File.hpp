//
// Created by robin on 28.12.2020.
//

#ifndef POCKETLZMA_FILE_HPP
#define POCKETLZMA_FILE_HPP

#include "MemoryStream.hpp"
#include <fstream>
namespace plz
{
    class File
    {
        public:
            File() = delete;
            static inline std::vector<uint8_t> FromMemory(const void *data, size_t size);
            static inline void FromMemory(const void *data, size_t size, std::vector<uint8_t> &output);

            static inline std::vector<uint8_t> FromFile(const std::string &path);
            static inline FileStatus FromFile(const std::string &path, std::vector<uint8_t> &output);

            static inline FileStatus ToFile(const std::string &path, const std::vector<uint8_t> &data);
    };

    std::vector<uint8_t> File::FromMemory(const void *data, size_t size)
    {
        std::vector<uint8_t> bytes(size);
        FromMemory(data, size, bytes);
        return bytes;
    }

    void File::FromMemory(const void *data, size_t size, std::vector<uint8_t> &output)
    {
        output.resize(size);
        plz::MemoryStream mem {(uint8_t *)data, size};
        mem.read((char *)&output[0], size);
    }

    std::vector<uint8_t> File::FromFile(const std::string &path)
    {
        std::fstream file;
        file = std::fstream(path, std::ios::in | std::ios::binary);

        //Find size
        file.seekg(0, std::ios::end);
        size_t fileSize = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<uint8_t> bytes(fileSize);

        file.read((char *)&bytes[0], fileSize);
        file.close();

        return bytes;
    }

    FileStatus File::FromFile(const std::string &path, std::vector<uint8_t> &output)
    {
        std::fstream file;
        try
        {
            file = std::fstream(path, std::ios::in | std::ios::binary);
            file.exceptions(std::fstream::failbit | std::fstream::badbit);
            bool isBad = file.bad();
            bool isFail = file.fail();
            bool isOpen = file.is_open();

            if(isBad || isFail || !isOpen)
                file.close();

            if(isBad)
                return FileStatus(FileStatus::Code::FileWriteErrorBadBit, 0, "", "", "");
            else if(isFail)
                return FileStatus(FileStatus::Code::FileWriteErrorFailBit, 0, "", "", "");
            else if(!isOpen)
                return FileStatus(FileStatus::Code::FileWriteError, 0, "", "", "");

            //Find size
            file.seekg(0, std::ios::end);
            size_t fileSize = file.tellg();
            file.seekg(0, std::ios::beg);

            output.resize(fileSize);

            file.read((char *) &output[0], fileSize);
            file.close();

            return FileStatus();
        }
        catch (const std::fstream::failure &e)
        {
            if(file.is_open())
                file.close();

            return FileStatus(FileStatus::Code::FileReadError, e.code().value(), e.what(), e.code().category().name(), e.code().message());
        }
    }

    FileStatus File::ToFile(const std::string &path, const std::vector<uint8_t> &data)
    {
        std::fstream file;
        file.exceptions(std::fstream::failbit | std::fstream::badbit);

        try
        {
            file = std::fstream(path, std::ios::out | std::ios::binary);
            bool isBad = file.bad();
            bool isFail = file.fail();
            bool isOpen = file.is_open();

            if(isBad || isFail || !isOpen)
                file.close();

            if(isBad)
                return FileStatus(FileStatus::Code::FileWriteErrorBadBit, 0, "", "", "");
            else if(isFail)
                return FileStatus(FileStatus::Code::FileWriteErrorFailBit, 0, "", "", "");
            else if(!isOpen)
                return FileStatus(FileStatus::Code::FileWriteError, 0, "", "", "");

            for (const auto &b : data) //b = byte
                file << b;

            file.close();
            return FileStatus();
        }
        catch (const std::fstream::failure &e)
        {
            if(file.is_open())
                file.close();

            return FileStatus(FileStatus::Code::FileWriteError, e.code().value(), e.what(), e.code().category().name(), e.code().message());
        }
    }
}

#endif //POCKETLZMA_FILE_HPP
