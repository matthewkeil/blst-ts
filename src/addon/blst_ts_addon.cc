#include <napi.h>
#include <sodium.h>
#include "vmas_worker.h"

Napi::Value verifyMultipleAggregateSignatures(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Array signatureSets = info[0].As<Napi::Array>();
    if (!signatureSets.IsArray())
    {
        Napi::Error err = Napi::Error::New(env, "signatureSets must be an array");
        err.ThrowAsJavaScriptException();
    }

    VerifyMultipleAggregateSignaturesWorker *worker = new VerifyMultipleAggregateSignaturesWorker(env, signatureSets);
    worker->Queue();
    return worker->GetPromise();
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
