#ifndef ZLIB_UTIL_H
#define ZLIB_UTIL_H
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "zlib.h"

class Zlib
{
public:
    static int def(uint8_t*, uint32_t, uint8_t*, uint32_t, int);
    static int inf(uint8_t*, uint32_t, uint8_t*, uint32_t);
private:
    Zlib();
};

#endif // ZLIB_H
