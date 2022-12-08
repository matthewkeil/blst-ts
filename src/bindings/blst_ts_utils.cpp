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
const char *get_blst_error_string(blst::BLST_ERROR err)
{
    return BLST_ERROR_STRINGS[err];
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

uint8_t ByteArray::CharToUnit8(const char &input)
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

uint8_t ByteArray::HexCharTupleToUint8(const char &char1, const char &char2)
{
    return (CharToUnit8(char1) << 4) | CharToUnit8(char2);
}

/**
 *
 * Copy Constructor
 *
 */
// ByteArray::ByteArray(const ByteArray &source)
//     // : byte_length{source.byte_length},
//     //   has_data_copy{source.has_data_copy}
// {
//     std::cout << "copy constructor: " << source.byte_length << std::endl;
//     data = source.data;
// }

/**
 *
 * Copy Assignment
 *
 */
// ByteArray &ByteArray::operator=(const ByteArray &rhs)
// {
//     std::cout << "copy assignment" << std::endl;
//     if (this != &rhs)
//     {
//         data.reset();
//         data = std::move(rhs.data);
//         byte_length = rhs.byte_length;
//         has_data_copy = rhs.has_data_copy;
//     }
//     return *this;
// }

/**
 *
 * Move Constructor
 *
 */
ByteArray::ByteArray(ByteArray &&source)
    : byte_length{source.byte_length},
      has_data_copy{source.has_data_copy}
{
    if (data != nullptr && byte_length > 0)
    {
        data.reset();
    }
    data = std::move(source.data);
    source.data = nullptr;
    source.has_data_copy = false;
    source.byte_length = 0;
}

/**
 *
 * Move Assignment
 *
 */
// ByteArray &ByteArray::operator=(ByteArray &&source)
// {
//     if (this != &source)
//     {
//         std::cout << "move assignment" << std::endl;
//         if (data != nullptr && byte_length > 0)
//         {
//             data.reset();
//         }
//         std::cout << "attempting" << std::endl;
//         data = std::move(source.data);
//         std::cout << "after" << std::endl;
//         source.data = nullptr;
//         byte_length = source.byte_length;
//         source.byte_length = 0;
//         has_data_copy = source.has_data_copy;
//         source.has_data_copy = false;
//         std::cout << "move assignment complete" << std::endl;
//     }
//     return *this;
// }

ByteArray::ByteArray(const char *in,
                     size_t in_len)
    : byte_length{0},
      has_data_copy{true}
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
    byte_length = (odd_number ? in_len + 1 : in_len) / 2;
    data = make_shared_array<blst::byte>(byte_length);
    size_t out_index = 0;
    data.get()[out_index] = HexCharTupleToUint8(char1, char2); // set first element
    out_index++;                                               // manually increment output iterator

    // loop through the rest of the tuples and convert to byte array
    for (; (in_index + 1) < in_len; out_index++, in_index += 2)
    {
        char1 = *(in + in_index);
        char2 = *(in + in_index + 1);
        if (out_index < byte_length)
        {
            data.get()[out_index] = HexCharTupleToUint8(char1, char2);
        }
    }
}

ByteArray::ByteArray(const uint8_t *const in, size_t in_len, bool save_copy)
    : byte_length{in_len},
      has_data_copy{save_copy}
{
    if (save_copy)
    {
        data = make_shared_array<blst::byte>(byte_length);
        for (size_t i = 0; i < byte_length; i++)
        {
            *(data.get() + i) = *(in + i);
        }
    }
}

ByteArray::ByteArray(const Napi::TypedArrayOf<uint8_t> &in, bool save_copy)
    : byte_length{in.ByteLength()},
      has_data_copy{save_copy}
{
    if (save_copy)
    {
        data = make_shared_array<blst::byte>(byte_length);
        for (size_t i = 0; i < byte_length; i++)
        {
            *(data.get() + i) = in[i];
        }
    }
}

void ByteArray::Clear()
{
    if (has_data_copy)
    {
        data.reset();
        has_data_copy = false;
    }
    byte_length = 0;
}

size_t ByteArray::ByteLength()
{
    return byte_length;
};

const blst::byte *ByteArray::Data()
{
    return data.get();
}

void ByteArray::ToString(std::string &out, bool should_prefix = false)
{
    static const char *hex = {"0123456789abcdef"};
    const auto data_ptr = data.get();
    out.clear();
    out.resize(2 * byte_length + (should_prefix ? 2 : 0));
    size_t out_it = 0;
    if (should_prefix)
    {
        out[0] = '0';
        out[1] = 'x';
        out_it += 2;
    }
    for (size_t i = 0; i < byte_length; i++, out_it += 2)
    {
        out[out_it] = hex[(*(data_ptr + i) >> 4) & 0xF];
        out[out_it + 1] = hex[(*(data_ptr + i)) & 0xF];
    }
}

Napi::Buffer<uint8_t> ByteArray::AsNapiBuffer(Napi::Env &env)
{
    return Napi::Buffer<uint8_t>::Copy(env, data.get(), byte_length);
};

Napi::String ByteArray::AsNapiString(Napi::Env &env)
{
    std::string str(byte_length, '0');
    ToString(str);
    return Napi::String::New(env, str);
};

Napi::TypedArrayOf<uint8_t> ByteArray::AsNapiUint8Array(Napi::Env &env)
{
    auto arr = Napi::TypedArrayOf<uint8_t>::New(env, byte_length, napi_uint8_array);
    for (size_t i = 0; i < byte_length; i++)
    {
        arr[i] = *(data.get() + i);
    }
    return arr;
};