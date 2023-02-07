#ifndef BLST_TS_UTILS_H__
#define BLST_TS_UTILS_H__

#include <sstream>
#include "napi.h"
#include "addon.h"

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

typedef enum
{
    Affine,
    Jacobian
} CoordType;

class BlstAsyncWorker : public Napi::AsyncWorker
{
public:
    BlstAsyncWorker(const Napi::CallbackInfo &info) : Napi::AsyncWorker{info.Env()},
                                                      _module{info.Env().GetInstanceData<BlstTsAddon>()},
                                                      _info{info},
                                                      _env{Env()},
                                                      _deferred{_env},
                                                      _use_deferred{false},
                                                      _threw_error{false} {};
    virtual void Setup(){};
    virtual Napi::Value GetReturnValue() { return _env.Undefined(); };

    Napi::Value RunSync()
    {
        Napi::HandleScope scope(_env);
        Setup();
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
        Napi::AsyncWorker::Queue();
        return GetPromise();
    };

protected:
    BlstTsAddon *_module;
    const Napi::CallbackInfo &_info;
    const Napi::Env _env;

    void OnOK() override
    {
        if (_use_deferred)
        {
            _deferred.Resolve(GetReturnValue());
        }
    }
    void OnError(Napi::Error const &err) override
    {
        if (_use_deferred)
        {
            _deferred.Reject(err.Value());
        }
        else
        {
            _threw_error = true;
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

#endif /* BLST_TS_UTILS_H__ */
