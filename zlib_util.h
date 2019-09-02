#ifndef ZLIB_UTIL_H
#define ZLIB_UTIL_H

#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include "zlib.h"

class Zlib
{
public:
    static int def(std::vector<uint8_t>&, std::vector<uint8_t>&, int);
    static int inf(std::vector<uint8_t>&, std::vector<uint8_t>&);
private:
    Zlib();
};

#endif // ZLIB_H
