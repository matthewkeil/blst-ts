#ifndef BLST_TS_UTILS_H__
#define BLST_TS_UTILS_H__

#include <openssl/rand.h>
#include <sstream>
#include "napi.h"
#include "addon.h"
#include "macros.h"

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
    Napi::Value RunSync();
    Napi::Value Run();

protected:
    const Napi::CallbackInfo &_info;
    Napi::Env _env;
    BlstTsAddon *_module;

    virtual void Setup() = 0;
    virtual Napi::Value GetReturnValue() = 0;

    virtual void SetError(const std::string &err);

    void virtual OnOK() override final;
    void virtual OnError(Napi::Error const &err) override final;
    Napi::Promise GetPromise();

private:
    Napi::Promise::Deferred _deferred;
    bool _use_deferred;
    bool _threw_error;
};

class Uint8ArrayArg
{
public:
    Uint8ArrayArg(const Napi::Env &env) : _env{env},
                                          _error{},
                                          _data{nullptr},
                                          _byte_length{0} {};
    Uint8ArrayArg(
        const Napi::Env &env,
        const Napi::Value &val,
        const std::string &err_prefix,
        size_t length1 = 0,
        size_t length2 = 0);
    Uint8ArrayArg(
        const Napi::CallbackInfo &info,
        size_t arg_position,
        const std::string &err_prefix,
        size_t length1 = 0,
        size_t length2 = 0) : Uint8ArrayArg{info.Env(),
                                            info[arg_position],
                                            err_prefix,
                                            length1,
                                            length2} {}

    /**
     * NOTE: There are some places in the code where it looks like there is a
     * copy operation but they are strictly disallowed, this is a move only class.
     * The compiler recognizes the ares with elision and will allow them to compile
     */
    Uint8ArrayArg &operator=(const Uint8ArrayArg &source) = delete;
    Uint8ArrayArg(const Uint8ArrayArg &source) = delete;
    Uint8ArrayArg &operator=(Uint8ArrayArg &&source) = default;
    Uint8ArrayArg(Uint8ArrayArg &&source) = default;
    uint8_t operator[](size_t index);

    const uint8_t *Data();
    size_t ByteLength();
    // void ThrowCppExceptionIfError();
    // void ThrowJsExceptionIfError();
    bool HasError() { return _error.size() > 0; };
    std::string GetError() { return _error; };

protected:
    Napi::Env _env;
    std::string _error;

    void SetError(const std::string &err) { _error = err; };

private:
    uint8_t *_data;
    size_t _byte_length;
    Napi::Reference<Napi::Uint8Array> _ref;

    bool ValidateLength(const std::string &_error_prefix, size_t length1, size_t length2);
};

class Uint8ArrayArgArray : public std::vector<Uint8ArrayArg>
{
public:
    Uint8ArrayArgArray() : std::vector<Uint8ArrayArg>{} {}
    Uint8ArrayArgArray(
        const Napi::Env &env,
        const Napi::Value &arr_val,
        const std::string &err_prefix_singular,
        const std::string &err_prefix_plural,
        size_t length1 = 0,
        size_t length2 = 0);
    Uint8ArrayArgArray(
        const Napi::CallbackInfo &info,
        size_t arg_position,
        const std::string &err_prefix_singular,
        const std::string &err_prefix_plural,
        size_t length1 = 0,
        size_t length2 = 0)
        : Uint8ArrayArgArray{info.Env(),
                             info[arg_position],
                             err_prefix_singular,
                             err_prefix_plural,
                             length1,
                             length2} {}

    /**
     * NOTE: There are some places in the code where it looks like there is a
     * copy operation but they are strictly disallowed, this is a move only class.
     * The compiler recognizes the ares with elision and will allow them to compile
     */
    Uint8ArrayArgArray(Uint8ArrayArgArray &&source) = default;
    Uint8ArrayArgArray &operator=(Uint8ArrayArgArray &&source) = default;
    Uint8ArrayArgArray(const Uint8ArrayArgArray &source) = delete;
    Uint8ArrayArgArray &operator=(const Uint8ArrayArgArray &source) = delete;
};

#endif /* BLST_TS_UTILS_H__ */
