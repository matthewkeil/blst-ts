#include <napi.h>
#include <sodium.h>
#include "blst_ts_utils.hpp"
#include "secret_key.hpp"
#include "public_key.hpp"
#include "signature.hpp"
// #include "functions/functions.hpp"

Napi::Object InitFunctions(Napi::Env env, Napi::Object exports);

Napi::Object Init(Napi::Env env, Napi::Object exports)
{
    if (sodium_init() < 0)
    {
        Napi::Error err = Napi::Error::New(env, "Error initializing libsodium");
        err.ThrowAsJavaScriptException();
    }

    // InitFunctions(env, exports);
    SecretKey::Init(env, exports);
    PublicKey::Init(env, exports);
    Signature::Init(env, exports);
    exports.Set(Napi::String::New(env, "SECRET_KEY_LENGTH"),
                Napi::Number::New(env, SECRET_KEY_LENGTH));
    exports.Set(Napi::String::New(env, "PUBLIC_KEY_LENGTH_UNCOMPRESSED"),
                Napi::Number::New(env, PUBLIC_KEY_LENGTH_UNCOMPRESSED));
    exports.Set(Napi::String::New(env, "PUBLIC_KEY_LENGTH_COMPRESSED"),
                Napi::Number::New(env, PUBLIC_KEY_LENGTH_COMPRESSED));
    exports.Set(Napi::String::New(env, "SIGNATURE_LENGTH_UNCOMPRESSED"),
                Napi::Number::New(env, SIGNATURE_LENGTH_UNCOMPRESSED));
    exports.Set(Napi::String::New(env, "SIGNATURE_LENGTH_COMPRESSED"),
                Napi::Number::New(env, SIGNATURE_LENGTH_COMPRESSED));
    exports.Set(Napi::String::New(env, "DST"),
                Napi::String::New(env, DST));

    return exports;
}

NODE_API_MODULE(addon, Init)
