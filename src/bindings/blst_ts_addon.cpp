#include <napi.h>
#include <sodium.h>
#include "secret_key.hpp"

Napi::Value test(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::TypeError::New(env, "error yo!").ThrowAsJavaScriptException();
    return Napi::String::New(env, "hello world");
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    if (sodium_init() < 0)
    {
        Napi::Error err = Napi::Error::New(env, "Error initializing libsodium");
        err.ThrowAsJavaScriptException();
    }

    SecretKey::Init(env, exports);
    exports.Set(Napi::String::New(env, "test"),
                Napi::Function::New(env, test));

    return exports;
}

NODE_API_MODULE(addon, Init)
