#ifndef __BLST_TS_UTILS_HPP__
#define __BLST_TS_UTILS_HPP__

#include <iterator>
#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <math.h>
// #include <sodium.h>
#include "napi.h"
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
const char *get_blst_error_string(blst::BLST_ERROR err);

class ByteArray
{
public:
    static ByteArray RandomBytes(size_t length, bool non_zero = false);
    ByteArray() : data{} {}
    ByteArray(const std::string &in) : ByteArray(in.c_str(), in.length()) {}
    ByteArray(const char *const in, const size_t in_len);
    ByteArray(const uint8_t *const in, const size_t in_len);
    ByteArray(const Napi::TypedArrayOf<uint8_t> &in);
    ByteArray(const Napi::String &in) : ByteArray{in.Utf8Value()} {}
    ByteArray(ByteArray &&source) = default;
    ByteArray(const ByteArray &source) = default;
    ByteArray &operator=(ByteArray &&source) = default;
    ByteArray &operator=(const ByteArray &source) = default;

    void Clear();
    size_t ByteLength();
    const blst::byte *Data();
    void ToString(std::string &out, bool should_prefix = false);
    Napi::String AsNapiString(Napi::Env &env);
    Napi::Buffer<uint8_t> AsNapiBuffer(Napi::Env &env);
    Napi::TypedArrayOf<uint8_t> AsNapiUint8Array(Napi::Env &env);

private:
    static uint8_t CharToUnit8(const char &input);
    static uint8_t HexCharTupleToUint8(const char &char1, const char &char2);

    std::vector<blst::byte> data;
};

#endif /* __BLST_TS_UTILS_HPP__ */