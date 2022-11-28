#include "blst_ts_utils.hpp"

const std::string SECRET_KEY_TYPE = "SecretKey";
const std::string PUBLIC_KEY_TYPE = "PublicKey";
const std::string SIGNATURE_TYPE = "Signature";

const std::string DST{"BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"};

const char *BLST_ERROR_STRINGS[] = {
    "BLST_SUCCESS",
    "BLST_BAD_ENCODING",
    "BLST_POINT_NOT_ON_CURVE",
    "BLST_POINT_NOT_IN_GROUP",
    "BLST_AGGR_TYPE_MISMATCH",
    "BLST_VERIFY_FAIL",
    "BLST_PK_IS_INFINITY",
    "BLST_BAD_SCALAR",
};

void random_bytes_non_zero(blst::byte *out, const size_t length)
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

void no_zero_bytes(uint8_t *in, const size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (in[i] == 0)
        {
            in[i] = 1;
        }
    }
};

const char *get_blst_error_string(blst::BLST_ERROR err)
{
    return BLST_ERROR_STRINGS[err];
};

void bytes_to_hex_string(std::string &out, blst::byte *in, size_t in_size)
{
    if (out.length() != in_size * 2)
    {
        throw std::invalid_argument("out string and in bytes are different lengths");
    }
    const char *hex = "0123456789ABCDEF";
    for (size_t i = 0; i < in_size; i++)
    {
        out[2 * i] = hex[(*(in + i) >> 4) & 0xF];
        out[2 * i + 1] = hex[*(in + i) & 0xF];
    }
}

uint8_t char_to_uint8(char &input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    throw std::invalid_argument("Invalid input string");
}

blst::byte join_hex_string_bytes(char &char1, char &char2)
{
    return (char_to_uint8(char1) << 4) | char_to_uint8(char2);
}

void hex_string_to_bytes(blst::byte *out, size_t out_len, const char *in, size_t in_len)
{
    size_t in_index = 0;
    char firstTwo[2] = {in[0], in[1]};
    bool prefixed = strcmp("0x", firstTwo);
    if (prefixed)
    {
        in_len -= 2;
        in_index += 2;
    }
    bool odd_number = in_len % 2 != 0;
    char char1 = odd_number ? '0' : in[in_index];
    char char2 = odd_number ? in[in_index] : in[in_index + 1];
    in_index += odd_number ? 1 : 2;
    if ((odd_number ? in_len + 1 : in_len) / 2 != out_len)
    {
        throw std::invalid_argument("hex_string_to_bytes: out_len and in_len dont match");
    }
    blst::byte *out_index = out;
    while (in_index + 2 <= in_len)
    {
        *out_index = join_hex_string_bytes(char1, char2);
        out_index++;
        in_index += 2;
        if (in_index != in_len)
        {
            char1 = in[in_index];
            char2 = in[in_index + 1];
        }
    }
}

void hex_string_to_bytes(blst::byte *out, size_t out_len, std::string &in)
{
    return hex_string_to_bytes(out, out_len, in.c_str(), in.size());
}