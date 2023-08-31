#include "string.h"

#if defined (__GNUC__)
__asm__(".section .bootloader_section,\"a\";"
    ".incbin \"../../cmake-build-debug-mingw/bare-metal-series-bootloader.bin\";");
#endif
