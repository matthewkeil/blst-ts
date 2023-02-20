/**
 * Processing props
 *
 *
 * Napi::TypedArrayOf<u_int8_t>
 *
 * void returns on error
 */
#define ARG_TO_UINT8_RETURN_VOID(INFO, ENV, NUM, NAME, ERROR_PREFIX)         \
    Napi::Value NAME##_val = INFO[NUM].As<Napi::Value>();                    \
    if (!NAME##_val.IsTypedArray())                                          \
    {                                                                        \
        THROW_ERROR_RETURN_VOID(ENV, #ERROR_PREFIX " must be a Uint8Array"); \
    }                                                                        \
    Napi::TypedArray NAME##_untyped = NAME##_val.As<Napi::TypedArray>();     \
    if (NAME##_untyped.TypedArrayType() != napi_uint8_array)                 \
    {                                                                        \
        THROW_ERROR_RETURN_VOID(ENV, #ERROR_PREFIX " must be a Uint8Array"); \
    }                                                                        \
    Napi::TypedArrayOf<u_int8_t> NAME = NAME##_untyped.As<Napi::TypedArrayOf<u_int8_t>>();

#define THROW_ERROR_RETURN_VOID(ENV, MSG)                    \
    Napi::Error::New(ENV, MSG).ThrowAsJavaScriptException(); \
    return;

#define CHECK_UINT8_LENGTH_RETURN_VOID(ENV, ARR, LENGTH, ERROR_PREFIX)                 \
    if (ARR.ByteLength() != LENGTH)                                                    \
    {                                                                                  \
        THROW_ERROR_RETURN_VOID(ENV, #ERROR_PREFIX " must be " #LENGTH " bytes long"); \
    }

#define ARG_UINT8_OF_LENGTH_RETURN_VOID(INFO, ENV, NUM, NAME, LENGTH, ERROR_PREFIX) \
    ARG_TO_UINT8_RETURN_VOID(INFO, ENV, NUM, NAME, ERROR_PREFIX)                    \
    CHECK_UINT8_LENGTH_RETURN_VOID(ENV, NAME, LENGTH, ERROR_PREFIX)

/**
 * Napi::TypedArrayOf<u_int8_t>
 *
 * Undefined() returns on error
 */
#define THROW_ERROR_UNDEFINED(ENV, MSG)                      \
    Napi::Error::New(ENV, MSG).ThrowAsJavaScriptException(); \
    return ENV.Undefined();

#define CHECK_UINT8_LENGTH_UNDEFINED(ENV, ARR, LENGTH, ERROR_PREFIX)                 \
    if (ARR.ByteLength() != LENGTH)                                                  \
    {                                                                                \
        THROW_ERROR_UNDEFINED(ENV, #ERROR_PREFIX " must be " #LENGTH " bytes long"); \
    }

#define CHECK_UINT8_2_LENGTHS_UNDEFINED(ENV, ARR, LENGTH1, LENGTH2, ERROR_PREFIX)                                            \
    size_t length = ARR.ByteLength();                                                                                        \
    if (!(length == LENGTH1 || length == LENGTH2))                                                                           \
    {                                                                                                                        \
        std::ostringstream msg;                                                                                              \
        msg << #ERROR_PREFIX << " is " << length << " bytes, but must be " << LENGTH1 << " or " << LENGTH2 << " bytes long"; \
        std::string msg_str = msg.str();                                                                                     \
        THROW_ERROR_UNDEFINED(ENV, msg_str);                                                                                 \
    }

#define ARG_TO_UINT8_UNDEFINED(INFO, ENV, NUM, NAME, ERROR_PREFIX)         \
    Napi::Value NAME##_val = INFO[NUM].As<Napi::Value>();                  \
    if (!NAME##_val.IsTypedArray())                                        \
    {                                                                      \
        THROW_ERROR_UNDEFINED(ENV, #ERROR_PREFIX " must be a Uint8Array"); \
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

#define UINT8_ARG_CHECK_2_LENGTHS_UNDEFINED(INFO, ENV, NUM, NAME, LENGTH1, LENGTH2, ERROR_PREFIX) \
    ARG_TO_UINT8_UNDEFINED(INFO, ENV, NUM, NAME, ERROR_PREFIX)                                    \
    CHECK_UINT8_2_LENGTHS_UNDEFINED(ENV, NAME, LENGTH1, LENGTH2, ERROR_PREFIX)

/**
 * Napi::TypedArrayOf<u_int8_t>
 *
 * Undefined() returns on error
 */

#define TYPE_AT_OBJ_KEY(OBJ, OBJ_NAME, KEY_NAME)                                       \
    if (!OBJ.Has(KEY_NAME))                                                            \
    {                                                                                  \
        throw Napi::Error::New(env, #OBJ_NAME " must have a '" #KEY_NAME "' property") \
    }                                                                                  \
    return obj.Get(key_name);
