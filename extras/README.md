Public Domain

This is a slightly modified cross-platform single-header version of Igor Pavlov's C-implementation of LZMA.
This is released under Public Domain to honor Igor Pavlov's hard work, which he himself put under Public Domain
for all of us to use as we please. So all credit for the LZMA logic here goes to Igor Pavlov himself.

Amalgamation (single-header) and cross-platform modifications done by Robin Berg Pettersen.
NOTE: In order to make this cross-platform, all multithreading logic had to be removed.

Use at own risk. **There is absolutely NO SUPPORT** for using this file. 

To be able to use this, you must `define "LZMA_C_DEFINE"` **ONCE** before including `"lzma_c.hpp"`, like this:
```c
#define LZMA_C_DEFINE
#include "lzma_c.hpp"
```
