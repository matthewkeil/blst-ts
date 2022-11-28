#ifndef __BLST_TS_UTILS_HH__
#define __BLST_TS_UTILS_HH__

#include <math.h>
#include <sodium.h>
#include "blst.hpp"

#ifndef SECRET_KEY_LENGTH
#define SECRET_KEY_LENGTH 32
#endif

#ifndef PUBLIC_KEY_LENGTH_COMPRESSED
#define PUBLIC_KEY_LENGTH_COMPRESSED 48
#endif

#ifndef PUBLIC_KEY_LENGTH_UNCOMPRESSED
#define PUBLIC_KEY_LENGTH_UNCOMPRESSED 96
#endif

#ifndef SIGNATURE_LENGTH_COMPRESSED
#define SIGNATURE_LENGTH_COMPRESSED 96
#endif

#ifndef SIGNATURE_LENGTH_UNCOMPRESSED
#define SIGNATURE_LENGTH_UNCOMPRESSED 192
#endif

typedef enum
{
    Affine,
    Jacobian
} CoordType;

extern const std::string SECRET_KEY_TYPE;
extern const std::string PUBLIC_KEY_TYPE;
extern const std::string SIGNATURE_TYPE;

extern const std::string DST;
extern const char *BLST_ERROR_STRINGS[];

void random_bytes_non_zero(blst::byte *out, const size_t length);
void no_zero_bytes(uint8_t *in, const size_t length);
const char *get_blst_error_string(blst::BLST_ERROR err);

void bytes_to_hex_string(std::string &out, blst::byte *in, size_t in_size);
void hex_string_to_bytes(blst::byte *out, size_t out_len, const char *in, size_t in_len);
void hex_string_to_bytes(blst::byte *out, size_t out_len, std::string &in);

#endif /* __BLST_TS_UTILS_HH__ */