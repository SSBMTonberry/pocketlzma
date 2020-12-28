//
// Created by robin on 19.12.2020.
//

#ifndef POCKETLZMA_LZMA_C_H
#define POCKETLZMA_LZMA_C_H

//Notes - LZMA-files
/*!
 * #include "Precomp.h"
 * #include "Lzma2Dec.h"
 * #include "Lzma2Dec.c"
 * #include "Lzma2DecMt.h"
 * #include "Alloc.h"
 * #include "MtDec.h"
 *
 *
 *
 */

namespace plz
{
    namespace c
    {
        //H
        #include "C/7zAlloc.h"
        #include "C/7zBuf.h"
        #include "C/7zCrc.h"
        #include "C/7zFile.h"
        #include "C/7z.h"
        #include "C/7zTypes.h"
        #include "C/7zVersion.h"
        #include "C/Aes.h"
        #include "C/Alloc.h"
        #include "C/Bcj2.h"
        #include "C/Bra.h"
        #include "C/Compiler.h"
        #include "C/CpuArch.h"
        #include "C/Delta.h"
        #include "C/DllSecur.h"
        #include "C/LzFind.h"
        #include "C/LzFindMt.h"
        #include "C/LzHash.h"
        #include "C/Lzma2Dec.h"
        #include "C/Lzma2DecMt.h"
        #include "C/Lzma2Enc.h"
        #include "C/Lzma86.h"
        #include "C/LzmaDec.h"
        #include "C/LzmaEnc.h"
        #include "C/LzmaLib.h"
        #include "C/MtCoder.h"
        #include "C/MtDec.h"
        //#include "C/Ppmd7.h"
        //#include "C/Ppmd.h"
        #include "C/Precomp.h"
        #include "C/RotateDefs.h"
        #include "C/Sha256.h"
        #include "C/Sort.h"
        #include "C/Threads.h"
        //#include "C/XzCrc64.h"
        //#include "C/XzEnc.h"
        //#include "C/Xz.h"

        //C

//        #include "C/7zAlloc.c"
//        #include "C/7zArcIn.c"
//        #include "C/7zBuf2.c"
//        #include "C/7zBuf.c"
//        #include "C/7zCrc.c"
//        #include "C/7zCrcOpt.c"
//        #include "C/7zDec.c"
//        #include "C/7zFile.c"
//        #include "C/7zStream.c"
//        #include "C/7zVersion.rc" //RBP: MUST add this extension to amalgamate script.
//        #include "C/Aes.c"
//        #include "C/AesOpt.c"
//        #include "C/Alloc.c"
//        #include "C/Bcj2.c"
//        #include "C/Bcj2Enc.c"
//        #include "C/Bra86.c"
//        #include "C/Bra.c"
//        #include "C/BraIA64.c"
//        #include "C/CpuArch.c"
//        #include "C/Delta.c"
//        #include "C/DllSecur.c"
//        #include "C/LzFind.c"
//        #include "C/LzFindMt.c"
//        #include "C/Lzma2Dec.c"
//        #include "C/Lzma2DecMt.c"
//        #include "C/Lzma2Enc.c"
//        #include "C/Lzma86Dec.c"
//        #include "C/Lzma86Enc.c"
//        #include "C/LzmaDec.c"
//        #include "C/LzmaEnc.c"
//        #include "C/LzmaLib.c"
//        #include "C/MtCoder.c"
//        #include "C/MtDec.c"
//        //#include "C/Ppmd7.c"
//        //#include "C/Ppmd7Dec.c"
//        //#include "C/Ppmd7Enc.c"
//        #include "C/Sha256.c"
//        #include "C/Sort.c"
//        #include "C/Threads.c"
//        //#include "C/Xz.c"
//        //#include "C/XzCrc64.c"
//        //#include "C/XzCrc64Opt.c"
//        //#include "C/XzDec.c"
//        //#include "C/XzEnc.c"
//        //#include "C/XzIn.c"
    }
}
#endif //POCKETLZMA_LZMA_C_H
