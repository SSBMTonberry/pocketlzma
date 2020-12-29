/*!
 *  BSD 2-Clause License

    Copyright (c) 2020, Robin Berg Pettersen
    All rights reserved.

    Redistribution and use in source and binary forms, with or without
    modification, are permitted provided that the following conditions are met:

    1. Redistributions of source code must retain the above copyright notice, this
       list of conditions and the following disclaimer.

    2. Redistributions in binary form must reproduce the above copyright notice,
       this list of conditions and the following disclaimer in the documentation
       and/or other materials provided with the distribution.

    THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
    AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
    IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
    DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
    FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
    DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
    SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
    CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
    OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
    OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 */

#ifndef POCKETLZMA_POCKETLZMA_H
#define POCKETLZMA_POCKETLZMA_H

namespace plz
{
    namespace c
    {
        #include "lzma_c/7zTypes.h"
        #include "lzma_c/Precomp.h"
        #include "lzma_c/Alloc.h"
        #include "lzma_c/LzmaDec.h"
        #include "lzma_c/LzmaEnc.h"

        #include "lzma_c/LzmaLib.h"

        #ifndef POCKETLZMA_LZMA_C_DEFINED
        #define POCKETLZMA_LZMA_C_DEFINED
            #include "lzma_c/Alloc.c"
            #include "lzma_c/LzmaDec.c"
            #include "lzma_c/LzmaEnc.c"
            #include "lzma_c/LzFind.c"
            #include "lzma_c/LzmaLib.c"
        #endif //POCKETLZMA_LZMA_C_DEFINED
    }
}
#include "pocketlzma_common.hpp"
#include "Settings.hpp"
#include "misc/FileStatus.hpp"
#include "misc/File.hpp"
#include "misc/MemoryBuffer.hpp"
#include "misc/MemoryStream.hpp"
#include "pocketlzma_class.hpp"

#endif //POCKETLZMA_POCKETLZMA_H
