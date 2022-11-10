#include <napi.h>
#include "sodium.h"

Napi::Value verifyMultipleAggregateSignatures(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    Napi::Uint8Array param1 = info[0].As<Napi::Uint8Array>();

    Napi::Promise::Deferred deferred = Napi::Promise::Deferred::New(env);

    return deferred.Promise();
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    if (sodium_init() < 0)
    {
        Napi::Error err = Napi::Error::New(env, "Error initializing libsodium");
        err.ThrowAsJavaScriptException();
    }

    exports.Set(Napi::String::New(env, "verifyMultipleAggregateSignatures"),
                Napi::Function::New(env, verifyMultipleAggregateSignatures));

    return exports;
}

NODE_API_MODULE(addon, Init)
