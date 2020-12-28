//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_POCKETLZMA_H
#define POCKETLZMA_POCKETLZMA_H

namespace plz
{
    namespace c
    {
        //Only single thread.
        #define _7ZIP_ST

        #include "lzma_c/7zTypes.h"
        #include "lzma_c/Precomp.h"
        #include "lzma_c/Alloc.h"
        #include "lzma_c/Alloc.c"
        #include "lzma_c/LzmaDec.h"
        #include "lzma_c/LzmaDec.c"
        #include "lzma_c/LzmaEnc.h"
        #include "lzma_c/LzmaEnc.c"

        #include "lzma_c/LzFind.c"
        //#include "lzma_c/Threads.h"
        //#include "lzma_c/Threads.c"
        //#include "lzma_c/LzFindMt.c"

        #include "lzma_c/LzmaLib.h"
        #include "lzma_c/LzmaLib.c"
    }
}


#endif //POCKETLZMA_POCKETLZMA_H
