#ifndef BLST_TS_UTILS_H
#define BLST_TS_UTILS_H

#include <sodium.h>
#include "blst.hpp"

void randomBytesNonZero(blst::byte *out, const size_t length)
{
    printf("in randomBytesNonZero\n");
    randombytes_buf(out, length);

    // for (size_t i = 0; i < length; i++)
    // {
    //     if (out[i] == 0)
    //     {
    //         out[i] = 1;
    //     }
    // }
};

#endif