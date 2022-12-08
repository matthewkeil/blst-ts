#ifndef __BLST_TS_UTILS_HH__
#define __BLST_TS_UTILS_HH__

#include <iterator>
#include <algorithm>
#include <memory>
#include <vector>
#include <iostream>
#include <math.h>
#include <sodium.h>
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

void random_bytes_non_zero(blst::byte *out, const size_t length);
void no_zero_bytes(uint8_t *in, const size_t length);
const char *get_blst_error_string(blst::BLST_ERROR err);

void bytes_to_hex_string(std::string &out, blst::byte *in, size_t in_size);

template <typename T>
std::shared_ptr<T> make_shared_array(size_t size)
{
    return std::shared_ptr<T>(new T[size], std::default_delete<T[]>());
}

class ByteArray
{
public:
    ByteArray() : byte_length{0}, has_data_copy{false}, data{nullptr} {}
    ByteArray(const std::string &in) : ByteArray(in.c_str(), in.length()) {}
    ByteArray(const char *const in, const size_t in_len);
    ByteArray(const uint8_t *const in, const size_t in_len, bool save_copy = true);
    ByteArray(const Napi::TypedArrayOf<uint8_t> &in, bool save_copy = true);
    ~ByteArray() { Clear(); }
    ByteArray(ByteArray &&source);
    ByteArray(const ByteArray &source) = default;
    ByteArray &operator=(ByteArray &&source) = delete;
    ByteArray &operator=(const ByteArray &source) = delete;

    void Clear();
    size_t ByteLength();
    const blst::byte *Data();
    void ToString(std::string &out, bool should_prefix);
    Napi::String AsNapiString(Napi::Env &env);
    Napi::Buffer<uint8_t> AsNapiBuffer(Napi::Env &env);
    Napi::TypedArrayOf<uint8_t> AsNapiUint8Array(Napi::Env &env);

private:
    size_t byte_length;
    bool has_data_copy = false;
    std::shared_ptr<blst::byte> data;

    const blst::byte *GetPointer();

    uint8_t static CharToUnit8(const char &input);
    uint8_t static HexCharTupleToUint8(const char &char1, const char &char2);
};

#endif /* __BLST_TS_UTILS_HH__ */