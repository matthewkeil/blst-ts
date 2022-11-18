#pragma once
// #ifndef __BLST_TS_UTILS_H__
// #define __BLST_TS_UTILS_H__

#include <sodium.h>
#include "blst_ts.hpp"

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

// #endif /* __BLST_TS_UTILS_H__ */