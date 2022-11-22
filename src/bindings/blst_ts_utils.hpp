#ifndef __BLST_TS_UTILS_HH__
#define __BLST_TS_UTILS_HH__

#include <sodium.h>
#include "blst.hpp"

static const char *BLST_ERROR_STRINGS[] = {
    "BLST_SUCCESS",
    "BLST_BAD_ENCODING",
    "BLST_POINT_NOT_ON_CURVE",
    "BLST_POINT_NOT_IN_GROUP",
    "BLST_AGGR_TYPE_MISMATCH",
    "BLST_VERIFY_FAIL",
    "BLST_PK_IS_INFINITY",
    "BLST_BAD_SCALAR",
};

inline void random_bytes_non_zero(blst::byte *out, const size_t length)
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

inline bool is_zero_bytes(uint8_t *in, const size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (in[i] != 0)
        {
            return true;
        }
    }
    return false;
};

inline const char *get_blst_error_string(blst::BLST_ERROR err)
{
    return BLST_ERROR_STRINGS[err];
};

    // inline void to_hex(blst::byte *in, long in_size, char *out, long out_size)
    // {
    //     blst::byte *pin = in;
    //     const char *hex = "0123456789ABCDEF";
    //     char *pout = out;
    //     for (; pin < in + in_size; pout += 3, pin++)
    //     {
    //         pout[0] = hex[(*pin >> 4) & 0xF];
    //         pout[1] = hex[*pin & 0xF];
    //         pout[2] = ':';
    //         if (pout + 3 - out > out_size)
    //         {
    //             /* Better to truncate output string than overflow buffer */
    //             /* it would be still better to either return a status */
    //             /* or ensure the target buffer is large enough and it never happen */
    //             break;
    //         }
    //     }
    //     pout[-1] = 0;
    // }

#endif /* __BLST_TS_UTILS_HH__ */