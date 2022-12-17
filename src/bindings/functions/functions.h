#include "napi.h"
#include "../blst_ts_utils.hpp"

Napi::Value AggregatePublicKeys(const Napi::CallbackInfo &info);

namespace ver_mult_agg_sigs
{
    Napi::Value VerifyMultipleAggregateSignatures(const Napi::CallbackInfo &info);
    Napi::Value VerifyMultipleAggregateSignaturesAsync(const Napi::CallbackInfo &info);
}

namespace agg_verify
{
    Napi::Value AggregateVerify(const Napi::CallbackInfo &info);
    Napi::Value AggregateVerifyAsync(const Napi::CallbackInfo &info);
}

Napi::Value TestBufferAsBuffer(const Napi::CallbackInfo &info);
Napi::Value TestBufferAsString(const Napi::CallbackInfo &info);
Napi::Value TestStringAsBuffer(const Napi::CallbackInfo &info);
Napi::Value TestTypedArrayAsTypedArray(const Napi::CallbackInfo &info);
Napi::Value TestTypedArrayAsString(const Napi::CallbackInfo &info);

Napi::Object InitFunctions(Napi::Env env, Napi::Object exports)
{
    Napi::Object functions = Napi::Object::New(env);
    functions.Set(Napi::String::New(env, "aggregatePublicKeys"),
                  Napi::Function::New(env, AggregatePublicKeys));
    functions.Set(Napi::String::New(env, "aggregateVerify"),
                  Napi::Function::New(env, agg_verify::AggregateVerify));
    functions.Set(Napi::String::New(env, "aggregateVerifyAsync"),
                  Napi::Function::New(env, agg_verify::AggregateVerifyAsync));
    functions.Set(Napi::String::New(env, "verifyMultipleAggregateSignatures"),
                  Napi::Function::New(env, ver_mult_agg_sigs::VerifyMultipleAggregateSignatures));
    functions.Set(Napi::String::New(env, "verifyMultipleAggregateSignaturesAsync"),
                  Napi::Function::New(env, ver_mult_agg_sigs::VerifyMultipleAggregateSignaturesAsync));

    Napi::Object tests = Napi::Object::New(env);
    tests.Set(Napi::String::New(env, "testBufferAsBuffer"),
              Napi::Function::New(env, TestBufferAsBuffer));
    tests.Set(Napi::String::New(env, "testBufferAsString"),
              Napi::Function::New(env, TestBufferAsString));
    tests.Set(Napi::String::New(env, "testTypedArrayAsTypedArray"),
              Napi::Function::New(env, TestTypedArrayAsTypedArray));
    tests.Set(Napi::String::New(env, "testTypedArrayAsString"),
              Napi::Function::New(env, TestTypedArrayAsString));
    tests.Set(Napi::String::New(env, "testStringAsBuffer"),
              Napi::Function::New(env, TestStringAsBuffer));
    functions.Set(Napi::String::New(env, "tests"), tests);

    exports.Set(Napi::String::New(env, "functions"), functions);
    return exports;
}
