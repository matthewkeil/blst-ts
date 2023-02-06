#ifndef BLST_TS_MACROS_H__
#define BLST_TS_MACROS_H__

#define NAPI_ENV Napi::Env env = info.Env();
// #define NAPI_FALSE(ENV) Napi::Boolean::New(ENV, false)
// #define NAPI_TRUE(ENV) Napi::Boolean::New(ENV, true)
// #define NAPI_NULL(ENV) ENV.Null()
// #define NAPI_UNDEFINED(ENV) ENV.Undefined();

#define THROW_ERROR_RETURN_VOID(ENV, MSG)                    \
    Napi::Error::New(ENV, MSG).ThrowAsJavaScriptException(); \
    return;

#define CHECK_UINT8_LENGTH_RETURN_VOID(ENV, ARR, LENGTH, ERROR_PREFIX)                 \
    if (ARR.ByteLength() != LENGTH)                                                    \
    {                                                                                  \
        THROW_ERROR_RETURN_VOID(ENV, #ERROR_PREFIX " must be " #LENGTH " bytes long"); \
    }

#define ARG_TO_UINT8_RETURN_VOID(INFO, ENV, NUM, NAME, ERROR_PREFIX)         \
    Napi::Value NAME##_val = INFO[NUM].As<Napi::Value>();                    \
    if (!NAME##_val.IsTypedArray())                                          \
    {                                                                        \
        THROW_ERROR_RETURN_VOID(ENV, #ERROR_PREFIX " must be a TypedArray"); \
    }                                                                        \
    Napi::TypedArray NAME##_untyped = NAME##_val.As<Napi::TypedArray>();     \
    if (NAME##_untyped.TypedArrayType() != napi_uint8_array)                 \
    {                                                                        \
        THROW_ERROR_RETURN_VOID(ENV, #ERROR_PREFIX " must be a Uint8Array"); \
    }                                                                        \
    Napi::TypedArrayOf<u_int8_t> NAME = NAME##_untyped.As<Napi::TypedArrayOf<u_int8_t>>();

#define ARG_UINT8_OF_LENGTH_RETURN_VOID(INFO, ENV, NUM, NAME, LENGTH, ERROR_PREFIX) \
    ARG_TO_UINT8_RETURN_VOID(INFO, ENV, NUM, NAME, ERROR_PREFIX)                    \
    CHECK_UINT8_LENGTH_RETURN_VOID(ENV, NAME, LENGTH, ERROR_PREFIX)

#define THROW_ERROR_UNDEFINED(ENV, MSG)                      \
    Napi::Error::New(ENV, MSG).ThrowAsJavaScriptException(); \
    return ENV.Undefined();

#define CHECK_UINT8_LENGTH_UNDEFINED(ENV, ARR, LENGTH, ERROR_PREFIX)                 \
    if (ARR.ByteLength() != LENGTH)                                                  \
    {                                                                                \
        THROW_ERROR_UNDEFINED(ENV, #ERROR_PREFIX " must be " #LENGTH " bytes long"); \
    }

#define ARG_TO_UINT8_UNDEFINED(INFO, ENV, NUM, NAME, ERROR_PREFIX)         \
    Napi::Value NAME##_val = INFO[NUM].As<Napi::Value>();                  \
    if (!NAME##_val.IsTypedArray())                                        \
    {                                                                      \
        THROW_ERROR_UNDEFINED(ENV, #ERROR_PREFIX " must be a TypedArray"); \
    }                                                                      \
    Napi::TypedArray NAME##_untyped = NAME##_val.As<Napi::TypedArray>();   \
    if (NAME##_untyped.TypedArrayType() != napi_uint8_array)               \
    {                                                                      \
        THROW_ERROR_UNDEFINED(ENV, #ERROR_PREFIX " must be a Uint8Array"); \
    }                                                                      \
    Napi::TypedArrayOf<u_int8_t> NAME = NAME##_untyped.As<Napi::TypedArrayOf<u_int8_t>>();

#define ARG_UINT8_OF_LENGTH_RETURN_UNDEFINED(INFO, ENV, NUM, NAME, LENGTH, ERROR_PREFIX) \
    ARG_TO_UINT8_UNDEFINED(INFO, ENV, NUM, NAME, ERROR_PREFIX)                           \
    CHECK_UINT8_LENGTH_UNDEFINED(ENV, NAME, LENGTH, ERROR_PREFIX)

//
//
//
//

// void check_uint8_length(
//     const Napi::Env &env,
//     Napi::Uint8Array &arr,
//     size_t length,
//     const std::string &prefix)
// {
//     if (arr.ByteLength() != length)
//     {
//         THROW_ERROR_RETURN_VOID(env, #ERROR_PREFIX " must be " #LENGTH " bytes long");
//     }
// }

// void arg_num_to_uint8(
//     Napi::Uint8Array &arr,
//     const Napi::CallbackInfo &info,
//     const Napi::Env &env,
//     size_t index,
//     const std::string &prefix)
// {
//     Napi::Value val = info[index].As<Napi::Value>();
//     if (!val.IsTypedArray())
//     {
//         THROW_ERROR_RETURN_VOID(env, #ERROR_PREFIX " must be a TypedArray");
//     }
//     Napi::TypedArray untyped = val.As<Napi::TypedArray>();
//     if (untyped.TypedArrayType() != napi_uint8_array)
//     {
//         THROW_ERROR_RETURN_VOID(env, #ERROR_PREFIX " must be a Uint8Array");
//     }
//     arr = untyped.As<Napi::TypedArrayOf<u_int8_t>>();
// }

// void arg_num_to_uint8_of_length(
//     Napi::Uint8Array &arr,
//     const Napi::CallbackInfo &info,
//     const Napi::Env &env,
//     size_t index,
//     size_t length,
//     const std::string &prefix)
// {
//     arg_num_to_uint8(arr, info, env, index, prefix);
//     check_uint8_length(env, arr, length, prefix);
// }

#endif