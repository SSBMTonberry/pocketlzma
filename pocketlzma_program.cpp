//
// Created by robin on 01.01.2021.
//

#define POCKETLZMA_LZMA_C_DEFINE
#include "pocketlzma.hpp"

//#include <cstdarg>  // For va_start, etc.

template<typename ... Args>
std::string fmt( const std::string& format, Args ... args )
{
    int size = snprintf( nullptr, 0, format.c_str(), args ... ) + 1; // Extra space for '\0'
    if( size <= 0 ){ throw std::runtime_error( "Error during formatting." ); }
    std::unique_ptr<char[]> buf( new char[ size ] );
    snprintf( buf.get(), size, format.c_str(), args ... );
    return std::string( buf.get(), buf.get() + size - 1 ); // We don't want the '\0' inside
}



const std::string VERSION = fmt("v%d.%d.%d", POCKETLZMA_VERSION_MAJOR, POCKETLZMA_VERSION_MINOR, POCKETLZMA_VERSION_PATCH);

void printHelp()
{
    std::cout << "Welcome to the PocketLzma program " << VERSION << "!\n"
              << "This program is intended to showcase how to use PocketLzma, but you can \n"
              << "also use it as a tool to compress/decompress files.\n\n"
              << "How to use: \n"
              << "<option> <input> <output>(optional - appending '.lzma' to <input> value when not used)\n"
              << "Example: -c ./file.json ./file.lzma\n\n"
              << "Options: \n"
              << "-h   | --help            - Print help \n"
              << "-d   | --decompress      - Decompress <input> LZMA file \n"
              << "-c   | --compress        - Compress with 'Default' preset \n"
              << "-cf+ | --fastest         - Compress with 'Fastest' preset \n"
              << "-cf  | --fast            - Compress with 'Fast' preset \n"
              << "-cg  | --good            - Compress with 'GoodCompression' preset \n"
              << "-cb  | --best            - Compress with 'BestCompression' preset \n\n";
}

void compress(const std::string &input, const std::string &output, plz::Preset preset)
{
    plz::PocketLzma p {preset};
}

void decompress(const std::string &input, const std::string &output)
{

}

int main(int argc, char *argv[])
{
    plz::PocketLzma p;
    if (argc > 1)
    {
        std::string action {argv[1]};
        if(action.length() > 0 && action[0] == '-')
        {

        }
        else
        {
            std::cout << "Invalid action: " << action << "\n\n";
            printHelp();
        }
    }
    else
        printHelp();
}