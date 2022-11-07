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

Napi::Number verifyMultipleAggregateSignatures(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    Napi::Uint8Array param1 = info[0].As<Napi::Uint8Array>();

    // size_t length = param1.ByteLength();

    uint8_t *data = param1.Data();
    const blst_p1_affine *p1Affine = blst_p1_affine_generator();

    BLST_ERROR error = blst_p1_deserialize(p1Affine, data);

    return Napi::Number::New(env, error);
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

    exports.Set(Napi::String::New(env, "randomBytes"), Napi::Buffer<char>::Copy(env, (char *)randomBytes, size));

    exports.Set(Napi::String::New(env, "verifyMultipleAggregateSignatures"),
                Napi::Function::New(env, verifyMultipleAggregateSignatures));

    return exports;
}

NODE_API_MODULE(addon, Init)
