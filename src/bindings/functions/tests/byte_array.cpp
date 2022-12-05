#include "napi.h"
#include "../../blst_ts_utils.hpp"

Napi::Value TestBufferAsBuffer(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (!info[0].IsBuffer())
    {
        Napi::TypeError::New(env, "TestBufferAsBuffer takes a buffer argument").ThrowAsJavaScriptException();
    }
    Napi::Buffer<uint8_t> testBuff = info[0].As<Napi::Buffer<uint8_t>>();
    ByteArray testArray{*testBuff.Data(), testBuff.ByteLength(), true};
    return testArray.AsNapiBuffer(env);
}

Napi::Value TestBufferAsString(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (!info[0].IsBuffer())
    {
        Napi::TypeError::New(env, "TestBufferAsString takes a buffer argument").ThrowAsJavaScriptException();
    }
    Napi::Buffer<uint8_t> testBuff = info[0].As<Napi::Buffer<uint8_t>>();
    ByteArray testArray{*testBuff.Data(), testBuff.ByteLength(), true};
    return testArray.AsNapiString(env);
}

Napi::Value TestTypedArrayAsTypedArray(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (!info[0].IsTypedArray())
    {
        Napi::TypeError::New(env, "TestTypedArrayAsTypedArray takes a Uint8Array argument").ThrowAsJavaScriptException();
    }
    Napi::TypedArrayOf<uint8_t> testBuff = info[0].As<Napi::TypedArrayOf<uint8_t>>();
    ByteArray testArray{*testBuff.Data(), testBuff.ByteLength(), true};
    return testArray.AsNapiBuffer(env);
}

Napi::Value TestTypedArrayAsString(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (!info[0].IsTypedArray())
    {
        Napi::TypeError::New(env, "TestTypedArrayAsString takes a buffer argument").ThrowAsJavaScriptException();
    }
    Napi::TypedArrayOf<uint8_t> testBuff = info[0].As<Napi::TypedArrayOf<uint8_t>>();
    ByteArray testArray{*testBuff.Data(), testBuff.ByteLength(), true};
    return testArray.AsNapiString(env);
}

Napi::Value TestStringAsBuffer(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "TestStringAsBuffer takes a string argument").ThrowAsJavaScriptException();
    }
    ByteArray testArray{info[0].As<Napi::String>().Utf8Value()};
    return testArray.AsNapiBuffer(env);
}