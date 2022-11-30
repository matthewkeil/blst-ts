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

void no_zero_bytes(blst::byte *out, const size_t length)
{
    for (size_t i = 0; i < length; i++)
    {
        if (out[i] == 0)
        {
            out[i] = 1;
        }
    }
};

void random_bytes_non_zero(blst::byte *out, const size_t length)
{
    randombytes_buf(out, length);
    no_zero_bytes(out, length);
};

const char *get_blst_error_string(blst::BLST_ERROR err)
{
    return BLST_ERROR_STRINGS[err];
};

uint8_t ByteArray::CharToUint8(const char &input)
{
    if (input >= '0' && input <= '9')
        return input - '0';
    if (input >= 'A' && input <= 'F')
        return input - 'A' + 10;
    if (input >= 'a' && input <= 'f')
        return input - 'a' + 10;
    printf("bad input: %c\n", input);
    throw std::invalid_argument("Invalid input string");
}

blst::byte ByteArray::JoinHexStringBytes(const char &char1, const char &char2)
{
    return (CharToUint8(char1) << 4) | CharToUint8(char2);
}

ByteArray::ByteArray(const blst::byte *in, size_t in_len, bool copy = false)
    : data{const_cast<blst::byte *>(in)}, byte_length{in_len}
{
    if (copy)
    {
        data = (blst::byte *)calloc(byte_length, sizeof(blst::byte));
        memcpy(data, in, byte_length);
    }
    else
    {
        should_delete = false;
    }
}
ByteArray::ByteArray(const char *in, size_t in_len)
    : data{nullptr}, byte_length{0}
{
    ByteArray::HexStringToBytes(data, &byte_length, in, in_len);
}

/**
 *
 * This function will calloc the array. The out_len is variable depending on how the
 * string is built.
 *
 * You MUST manually delete the blst::byte[] that is allocated or it will leak
 *
 */
void ByteArray::HexStringToBytes(blst::byte *out, size_t *out_len, const char *in, size_t in_len)
{
    // in_index is the byte to start reading from
    size_t in_index = 0;
    // if byte string is prefixed with the string value '0x' start reading from 3rd byte
    // and truncate in_length by length of truncated prefix
    if (*(in) == '0' && *(in + 1) == 'x')
    {
        in_len -= 2;
        in_index += 2;
    }
    // set first two characters depending on whether padding is needed
    // add padding '0' to front of odd length byte strings
    bool odd_number = in_len % 2 != 0;
    char char1 = odd_number ? '0' : *(in + in_index);
    char char2 = odd_number ? *(in + in_index) : *(in + in_index + 1);
    in_index += odd_number ? 1 : 2;

    // determine and return out_len to calling function
    *out_len = (odd_number ? in_len + 1 : in_len) / 2;
    // DELETE this memory after using byte string
    out = (blst::byte *)calloc(*out_len, sizeof(blst::byte));

    *out = JoinHexStringBytes(char1, char2);
    size_t out_index = 1; // manually increment output iterator
    // loop through the rest of the tuples and convert to byte array
    for (; (in_index + 1) < in_len; out_index++, in_index += 2)
    {
        char1 = *(in + in_index);
        char2 = *(in + in_index + 1);
        if (out_index < *out_len)
        {
            *(out + out_index) = JoinHexStringBytes(char1, char2);
        }
    }
}

void ByteArray::Clear()
{
    if (should_delete && (data != nullptr))
    {
        delete data;
    }
    data = nullptr;
    byte_length = 0;
}

size_t ByteArray::ByteLength()
{
    return byte_length;
};

blst::byte *ByteArray::Data()
{
    return data;
}

void ByteArray::ToString(std::string &out)
{
    static const char *hex = {"0123456789ABCDEF"};
    out.resize(2 * byte_length + 2);
    out.clear();
    out[0] = '0';
    out[1] = 'x';
    size_t out_it = 2;
    for (size_t i = 0; i < byte_length; i++, out_it += 2)
    {
        out[out_it] = hex[(*(data + i) >> 4) & 0xF];
        out[out_it + 1] = hex[(*(data + i)) & 0xF];
    }
}