#ifndef BLST_TS_UTILS_H__
#define BLST_TS_UTILS_H__

#include <sstream>
#include "napi.h"
#include "addon.h"

/**
 * Processing props
 *
 *
 * Napi::TypedArrayOf<u_int8_t>
 *
 * void returns on error
 */
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
        THROW_ERROR_RETURN_VOID(ENV, #ERROR_PREFIX " must be a Uint8Array"); \
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

typedef enum
{
    Affine,
    Jacobian
} CoordType;

class BlstAsyncWorker : public Napi::AsyncWorker
{
public:
    BlstAsyncWorker(const Napi::CallbackInfo &info) : Napi::AsyncWorker{info.Env()},
                                                      _info{info},
                                                      _env{Env()},
                                                      _module{_env.GetInstanceData<BlstTsAddon>()},
                                                      _deferred{_env},
                                                      _use_deferred{false},
                                                      _threw_error{false} {};
    Napi::Value RunSync()
    {
        Napi::HandleScope scope(_env);
        Setup();
        if (true)
        {
        }
        OnExecute(_env);
        SuppressDestruct();
        OnWorkComplete(_env, napi_ok);
        Napi::Value return_val = _threw_error ? _env.Undefined() : GetReturnValue();
        return return_val;
    }
    Napi::Value Run()
    {
        _use_deferred = true;
        Setup();
        Queue();
        return GetPromise();
    };

protected:
    const Napi::CallbackInfo &_info;
    Napi::Env _env;
    BlstTsAddon *_module;

    virtual void Setup() = 0;
    virtual Napi::Value GetReturnValue() = 0;

    virtual void SetError(const std::string &err)
    {
        _threw_error = true;
        Napi::AsyncWorker::SetError(err);
    }

    void virtual OnOK() override final
    {
        if (_use_deferred)
        {
            _deferred.Resolve(GetReturnValue());
        }
    }
    void virtual OnError(Napi::Error const &err) override final
    {
        if (_use_deferred)
        {
            _deferred.Reject(err.Value());
        }
        else
        {
            err.ThrowAsJavaScriptException();
        }
    }
    Napi::Promise GetPromise()
    {
        return _deferred.Promise();
    }

private:
    Napi::Promise::Deferred _deferred;
    bool _use_deferred;
    bool _threw_error;
};

class MessageArg
{
public:
    MessageArg(const Napi::Env &env, const Napi::Value &val) : _ref{}
    {
        if (val.IsTypedArray())
        {
            Napi::TypedArray untyped = val.As<Napi::TypedArray>();
            if (untyped.TypedArrayType() == napi_uint8_array)
            {
                Napi::Uint8Array arr = untyped.As<Napi::TypedArrayOf<u_int8_t>>();
                _ref = Napi::Persistent(arr);
                return;
            }
        }
        throw Napi::Error::New(env, "msg must be a BlstBuffer");
    }

    MessageArg(MessageArg &&source) = default;
    MessageArg &operator=(MessageArg &&source) = default;
    MessageArg(const MessageArg &source) = delete;
    MessageArg &operator=(const MessageArg &source) = delete;

    blst::byte *Data()
    {
        return _ref.Value().Data();
    }
    size_t ByteLength()
    {
        return _ref.Value().ByteLength();
    }

private:
    Napi::Reference<Napi::Uint8Array> _ref;
};

class MessageArgArray : public std::vector<MessageArg>
{
public:
    MessageArgArray() : std::vector<MessageArg>{} {}
    MessageArgArray(const Napi::Env &env, const Napi::Value &raw_arg) : MessageArgArray{}
    {
        if (!raw_arg.IsArray())
        {
            throw Napi::TypeError::New(env, "msgs must be of type BlstBuffer[]");
        }
        Napi::Array arr = raw_arg.As<Napi::Array>();
        for (size_t i = 0; i < arr.Length(); i++)
        {
            (*this)[i] = std::move(MessageArg{env, arr[i]});
        }
    }

    MessageArgArray(MessageArgArray &&source) = default;
    MessageArgArray &operator=(MessageArgArray &&source) = default;
    MessageArgArray(const MessageArgArray &source) = delete;
    MessageArgArray &operator=(const MessageArgArray &source) = delete;

    MessageArg &operator[](size_t index)
    {
        return (*this)[index];
    }
};

#endif /* BLST_TS_UTILS_H__ */
