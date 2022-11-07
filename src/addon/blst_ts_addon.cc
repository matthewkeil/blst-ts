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

    return exports;
}

NODE_API_MODULE(addon, Init)
