#include "blst_ts_utils.hpp"

const std::string SECRET_KEY_TYPE = "SecretKey";
const std::string PUBLIC_KEY_TYPE = "PublicKey";
const std::string SIGNATURE_TYPE = "Signature";

const std::string DST = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";

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


ByteArray ByteArray::RandomBytes(size_t length, bool non_zero)
{
    blst::byte bytes[length];
    randombytes_buf(bytes, length);
    if (non_zero)
    {
        for (size_t i = 0; i < length; i++)
        {
            if (bytes[i] == 0)
            {
                bytes[i] = 1;
            }
        }
    }
    return ByteArray{bytes, length};
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

ByteArray::ByteArray(const char *in,
                     size_t in_len)
    : data{}
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
    data.reserve((odd_number ? in_len + 1 : in_len) / 2);
    // data = make_shared_array<blst::byte>(byte_length);
    // size_t out_index = 0;
    data.push_back(HexCharTupleToUint8(char1, char2)); // set first element

    // loop through the rest of the tuples and convert to byte array
    for (; (in_index + 1) < in_len; in_index += 2)
        if (data.size() < data.capacity())
            data.push_back(HexCharTupleToUint8(*(in + in_index), *(in + in_index + 1)));
}

ByteArray::ByteArray(const uint8_t *const in, size_t in_len)
    : data{}
{
    data.resize(in_len);
    memcpy(&(data[0]), in, in_len);
}

ByteArray::ByteArray(const Napi::TypedArrayOf<uint8_t> &in)
    : data{}
{
    size_t length = in.ByteLength();
    data.resize(length);
    for (size_t i = 0; i < length; i++)
        data[i] = in[i];
}

void ByteArray::Clear()
{
    data.clear();
    data.resize(0);
}

size_t ByteArray::ByteLength()
{
    return data.size();
};

const blst::byte *ByteArray::Data()
{

    if (ByteLength() > 0)
    {
        return &(data[0]);
    }
    return nullptr;
}

void ByteArray::ToString(std::string &out, bool should_prefix)
{
    static const char *hex = {"0123456789abcdef"};
    size_t length = ByteLength();
    out.clear();
    out.resize(2 * length + (should_prefix ? 2 : 0));
    size_t out_it = 0;
    if (should_prefix)
    {
        out[out_it] = '0';
        out_it++;
        out[1] = 'x';
        out_it++;
    }
    std::for_each(data.begin(), data.end(), [&out, &out_it](blst::byte const &byte)
                  {
        out[out_it] = hex[(byte >> 4) & 0xF];
        out_it++;
        out[out_it] = hex[(byte) & 0xF];
        out_it++; });
}

Napi::Buffer<uint8_t> ByteArray::AsNapiBuffer(Napi::Env &env)
{
    return Napi::Buffer<uint8_t>::Copy(env, Data(), ByteLength());
};

Napi::String ByteArray::AsNapiString(Napi::Env &env)
{
    std::string str(ByteLength(), '0');
    ToString(str, false);
    return Napi::String::New(env, str);
};

Napi::TypedArrayOf<uint8_t> ByteArray::AsNapiUint8Array(Napi::Env &env)
{
    size_t length = ByteLength();
    auto arr = Napi::TypedArrayOf<uint8_t>::New(env, length, napi_uint8_array);
    for (size_t i = 0; i < length; i++)
        data[i] = arr[i];
    return arr;
};
