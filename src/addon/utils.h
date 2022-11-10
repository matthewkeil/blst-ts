#ifndef BLST_TS_UTILS_H
#define BLST_TS_UTILS_H

#include <sodium.h>

void randomBytesNonZero(uint8_t *out, const size_t length)
{
    randombytes_buf(out, length);

    for (size_t i = 0; i < length; i++)
    {
        if (out[i] == 0)
        {
            out[i] = 1;
        }
    }
};

#endif