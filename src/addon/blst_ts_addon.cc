#include "sodium.h"

#include "../../blst/bindings/blst.h"
#include "blst_ts_addon.h"

int randomBytesNonZero(byte *out, int length)
{
    randombytes_buf(out, length);

    for (int i = 0; i < length; i++)
    {
        if (out[i] == (byte)0)
        {
            out[i] = (byte)1;
        }
    }
};

BlstTsAddon::BlstTsAddon(const Napi::CallbackInfo &info) : ObjectWrap(info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return;
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "You need to name yourself")
            .ThrowAsJavaScriptException();
        return;
    }

    this->_greeterName = info[0].As<Napi::String>().Utf8Value();
}

Napi::Value BlstTsAddon::Greet(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (info.Length() < 1)
    {
        Napi::TypeError::New(env, "Wrong number of arguments")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    if (!info[0].IsString())
    {
        Napi::TypeError::New(env, "You need to introduce yourself to greet")
            .ThrowAsJavaScriptException();
        return env.Null();
    }

    Napi::String name = info[0].As<Napi::String>();

    printf("Hello %s\n", name.Utf8Value().c_str());
    printf("I am %s\n", this->_greeterName.c_str());

    return Napi::String::New(env, this->_greeterName);
}

Napi::Function BlstTsAddon::GetClass(Napi::Env env)
{
    return DefineClass(env, "BlstTsAddon", {
                                               BlstTsAddon::InstanceMethod("greet", &BlstTsAddon::Greet),
                                           });
}

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    if (sodium_init() < 0)
    {
        Napi::Error err = Napi::Error::New(env, "Error initializing libsodium");
        err.ThrowAsJavaScriptException();
    }

    int size = 32;
    byte randomBytes[size];
    randomBytesNonZero(randomBytes, size);

    exports.Set(Napi::String::New(env, "BlstTsAddon"), BlstTsAddon::GetClass(env));
    exports.Set(Napi::String::New(env, "randomBytes"), Napi::Buffer<char>::Copy(env, (char *)randomBytes, size));

    return exports;
}

NODE_API_MODULE(addon, Init)
