// Copyright Supranational LLC
// Licensed under the Apache License, Version 2.0, see LICENSE for details.
// SPDX-License-Identifier: Apache-2.0

%module blst
%rename("%(strip:[blst_])s") "";    // prefix is redundant in named module

%include "exception.i"
%include "std_string.i"
%typemap(out) SELF* OUTPUT = SWIGTYPE*; // to be overridden as required
%include "stdint.i"

%apply const char* { const byte*, const byte[ANY] }
%apply (const char *STRING, size_t LENGTH) { (const byte *STRING,
                                              size_t LENGTH) }

%header %{
#if V8_MAJOR_VERSION >= 8
# define GetData() GetBackingStore()->Data()
#else
# define GetData() GetContents().Data()
#endif
%}

%typemap(throws) BLST_ERROR %{ SWIG_V8_Raise(BLST_ERROR_str[$1]); SWIG_fail; %}

%typemap(in) const byte* %{
    if ($input->IsArrayBufferView()) {
        auto av = v8::Local<v8::ArrayBufferView>::Cast($input);
        auto buf = av->Buffer();
        $1 = ($1_ltype)buf->GetData() + av->ByteOffset();
    } else if ($input->IsNull()) {
        $1 = nullptr;
    } else {
        SWIG_exception_fail(SWIG_TypeError, "in method '$symname', "
                                            "expecting <Buffer>");
    }
%}
%typemap(argout)  const byte* ""
%typemap(freearg) const byte* ""

%apply const byte* { const byte[ANY] }
%typemap(in) const byte[ANY] %{
    if ($input->IsArrayBufferView()) {
        auto av = v8::Local<v8::ArrayBufferView>::Cast($input);
        if (av->ByteLength() != $1_dim0)
            SWIG_exception_fail(SWIG_IndexError, "in method '$symname', "
                                                 "expecting $1_dim0 bytes");
        auto buf = av->Buffer();
        $1 = ($1_ltype)buf->GetData() + av->ByteOffset();
    } else {
        SWIG_exception_fail(SWIG_TypeError, "in method '$symname', "
                                            "expecting <Buffer>");
    }
%}

// let users use JavaScript <BigInt> and <Buffer> as scalars
%typemap(in) (const byte* scalar, size_t nbits) %{
    if ($input->IsArrayBufferView()) {
        auto av = v8::Local<v8::ArrayBufferView>::Cast($input);
        auto buf = av->Buffer();
        $1 = ($1_ltype)buf->GetData() + av->ByteOffset();
        $2 = 8*av->ByteLength();
#if V8_MAJOR_VERSION >=6 && V8_MINOR_VERSION >= 8
    } else if ($input->IsBigInt()) {
        auto bi = v8::Local<v8::BigInt>::Cast($input);
        int sign, word_count = bi->WordCount();
        uint64_t* words = (uint64_t*)alloca($2 = word_count*sizeof(uint64_t));

        bi->ToWordsArray(&sign, &word_count, words);
        if (sign)
            SWIG_exception_fail(SWIG_TypeError, "in method '$symname', "
                                                "expecting unsigned value");
        $1 = ($1_ltype)words;
        $2 *= 8;

        const union {
            long one;
            char little;
        } is_endian = { 1 };

        if (!is_endian.little) {
            byte* p = $1;
            for (int i = 0; i < word_count; i++) {
                uint64_t val = words[i];
                for (size_t j = 0; j < sizeof(val); j++, val >>= 8)
                    *p++ = (byte)val;
            }
        }
#endif
    } else {
        SWIG_exception_fail(SWIG_TypeError, "in method '$symname', "
                                            "expecting <Buffer> or <BigInt>");
    }
%}

%typemap(in) (const byte *STRING, size_t LENGTH) %{
    if ($input->IsArrayBufferView()) {
        auto av = v8::Local<v8::ArrayBufferView>::Cast($input);
        auto buf = av->Buffer();
        $1 = ($1_ltype)buf->GetData() + av->ByteOffset();
        $2 = av->ByteLength();
    } else if ($input->IsString()) {
        auto str = v8::Local<v8::String>::Cast($input);
        $2 = SWIGV8_UTF8_LENGTH(str);
        $1 = ($1_ltype)alloca($2);
        SWIGV8_WRITE_UTF8(str, (char *)$1, $2);
    } else if ($input->IsNull()) {
        $1 = nullptr;
        $2 = 0;
    } else {
        SWIG_exception_fail(SWIG_TypeError, "in method '$symname', "
                                            "expecting <Buffer> or <String>");
    }
%}

// return |this|
%typemap(out) SELF* OUTPUT %{ (void)$1; $result = args.Holder(); %}

// everybody has a way to bundle pointer and buffer size, but C:-(
%apply (const byte *STRING, size_t LENGTH) {
       (const byte *msg,    size_t msg_len),
       (const byte *DST,    size_t DST_len),
       (const byte *aug,    size_t aug_len),
       (const byte *IKM,    size_t IKM_len),
       (const byte *info,   size_t info_len),
       (const byte *in,     size_t len)
}

// some sorcery to return byte[] from serialization methods
%typemap(in, numinputs=0) byte out[ANY] (byte temp[$1_dim0]) %{ $1 = temp; %}
%typemap(argout) byte out[ANY] {
    auto ab = v8::ArrayBuffer::New(v8::Isolate::GetCurrent(), $1_dim0);
    memcpy(ab->GetData(), $1, $1_dim0);
    $result = v8::Uint8Array::New(ab, 0, $1_dim0);
}
%typemap(freearg) byte out[ANY] ""
%apply byte out[ANY] {
    void to_bendian,    void blst_bendian_from_scalar,
    void to_lendian,    void blst_lendian_from_scalar,
    void serialize,     void blst_p1_serialize, void blst_p1_affine_serialize,
                        void blst_p2_serialize, void blst_p2_affine_serialize,
    void compress,      void blst_p1_compress,  void blst_p1_affine_compress,
                        void blst_p2_compress,  void blst_p2_affine_compress,
    void blst_sk_to_pk2_in_g1,  void blst_sign_pk2_in_g1,
    void blst_sk_to_pk2_in_g2,  void blst_sign_pk2_in_g2
}

%apply const std::string& { const std::string* }

#pragma SWIG nowarn=509,516

%ignore P1_Affines;
%ignore P2_Affines;

%ignore nullptr;
%ignore None;
%ignore C_bytes;
%catches(BLST_ERROR) P1(const byte* in, size_t len);
%catches(BLST_ERROR) P1_Affine(const byte* in, size_t len);
%catches(BLST_ERROR) aggregate(const P1_Affine& in);

%catches(BLST_ERROR) P2(const byte* in, size_t len);
%catches(BLST_ERROR) P2_Affine(const byte* in, size_t len);
%catches(BLST_ERROR) aggregate(const P2_Affine& in);

%catches(BLST_ERROR) blst::Scalar::add;
%catches(BLST_ERROR) blst::Scalar::sub;
%catches(BLST_ERROR) blst::Scalar::mul;

// methods returning |this|
%apply SELF* OUTPUT {
    blst::P1* sign_with,    blst::P2* sign_with,
    blst::P1* hash_to,      blst::P2* hash_to,
    blst::P1* encode_to,    blst::P2* encode_to,
    blst::P1* mult,         blst::P2* mult,
    blst::P1* cneg,         blst::P2* cneg,
    blst::P1* neg,          blst::P2* neg,
    blst::P1* add,          blst::P2* add,
    blst::P1* dbl,          blst::P2* dbl,
    blst::PT* mul,          blst::PT* sqr,
    blst::PT* final_exp,
    blst::Scalar* from_bendian,
    blst::Scalar* from_lendian,
    blst::Scalar* add,
    blst::Scalar* sub,
    blst::Scalar* mul,
    blst::Scalar* inverse
}

typedef enum {
    BLST_SUCCESS = 0,
    BLST_BAD_ENCODING,
    BLST_POINT_NOT_ON_CURVE,
    BLST_POINT_NOT_IN_GROUP,
    BLST_AGGR_TYPE_MISMATCH,
    BLST_VERIFY_FAIL,
    BLST_PK_IS_INFINITY,
} BLST_ERROR;

%include "blst.hpp"

extern const blst::P1_Affine BLS12_381_G1;
extern const blst::P1_Affine BLS12_381_NEG_G1;
extern const blst::P2_Affine BLS12_381_G2;
extern const blst::P2_Affine BLS12_381_NEG_G2;


%begin %{
# include <memory>
# include "blst.hpp"
using namespace blst;

static const char *const BLST_ERROR_str [] = {
    "BLST_ERROR: success",
    "BLST_ERROR: bad point encoding",
    "BLST_ERROR: point is not on curve",
    "BLST_ERROR: point is not in group",
    "BLST_ERROR: context type mismatch",
    "BLST_ERROR: verify failed",
    "BLST_ERROR: public key is infinite",
};

#define SWIG_PYTHON_STRICT_BYTE_CHAR

#if defined(__GNUC__)
# ifndef alloca
#  define alloca(s) __builtin_alloca(s)
# endif
#elif defined(__sun)
# include <alloca.h>
#elif defined(_WIN32)
# include <malloc.h>
# ifndef alloca
#  define alloca(s) _alloca(s)
# endif
#endif
%}

#if SWIG_VERSION < 0x040100 && defined(SWIGJAVASCRIPT)
%wrapper %{
#ifdef NODE_MODULE
# undef NODE_MODULE
# define NODE_MODULE NODE_MODULE_CONTEXT_AWARE
// actually error-prone and not exactly suitable for production, but
// sufficient for development purposes till SWIG 4.1.0 is released...
#endif
%}
#endif

