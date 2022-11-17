#ifndef BLST_TS_UTILS_H
#define BLST_TS_UTILS_H

#include <sodium.h>
#include "blst.hpp"

uint8_t randomBytesNonZero(blst::byte *out, const size_t length)
{
    if (sodium_init() < 0)
    {
        return 1;
    }

    randombytes_buf(out, length);

    return 0;
    // for (size_t i = 0; i < length; i++)
    // {
    //     if (out[i] == 0)
    //     {
    //         out[i] = 1;
    //     }
    // }
};

#endif