#include "secret_key.hpp"

#ifndef SECRET_KEY_LENGTH
#define SECRET_KEY_LENGTH 32
#endif

Napi::FunctionReference *SecretKey::constructor;
Napi::Object SecretKey::Init(Napi::Env env, Napi::Object exports)
{
    // make a pointer to the constructor function so it can be bound to the
    // lifetime of the addon module
    Napi::Function secretKeyConstructor = DefineClass(env, "SecretKey",
                                                      {
                                                          StaticMethod("keygen", &SecretKey::Keygen, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                          StaticMethod("fromBytes", &SecretKey::FromBytes, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                          //   InstanceMethod("toPublicKey", &SecretKey::ToPublicKey, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          //   InstanceMethod("sign", &SecretKey::Sign, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("toBytes", &SecretKey::ToBytes, static_cast<napi_property_attributes>(napi_enumerable)),
                                                      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(secretKeyConstructor);
    exports.Set(Napi::String::New(env, "SecretKey"), secretKeyConstructor);
    return exports;
}

SecretKey::SecretKey(const Napi::CallbackInfo &info) : Napi::ObjectWrap<SecretKey>(info), key{}, env{info.Env()}
{
    if (info[0].IsExternal())
    {
        auto passedKey = info[0].As<Napi::External<blst::SecretKey>>();
        key = *passedKey.Data();
        return;
    }
    else if (info.Length() > 0)
    {
        Napi::Error::New(env, "No arguments are allowed in SecretKey constructor").ThrowAsJavaScriptException();
        return;
    }
    blst::byte random[SECRET_KEY_LENGTH] = {};
    random_bytes_non_zero(random, SECRET_KEY_LENGTH);
    key.keygen(random, SECRET_KEY_LENGTH);
}

Napi::Value SecretKey::Keygen(const Napi::CallbackInfo &info)
{
    blst::SecretKey *key = new blst::SecretKey;
    Napi::Env env = info.Env();
    Napi::Value entropyVal = info[0].As<Napi::Value>();
    blst::byte ikm[SECRET_KEY_LENGTH] = {};

    if (entropyVal.IsUndefined())
    {
        random_bytes_non_zero(ikm, SECRET_KEY_LENGTH);
    }
    else if (!entropyVal.IsNull())
    {
        if (!entropyVal.IsTypedArray())
        {
            Napi::Error err = Napi::Error::New(env, "IKM for new SecretKey(ikm) must be a Uint8Array");
            err.ThrowAsJavaScriptException();
        }
        auto entropyArray = entropyVal.As<Napi::TypedArrayOf<u_int8_t>>();
        if (entropyArray.ByteLength() != SECRET_KEY_LENGTH)
        {
            Napi::Error err = Napi::Error::New(env, "ikm must be 32 bytes long");
            err.ThrowAsJavaScriptException();
        }
        memcpy(ikm, entropyArray.Data(), SECRET_KEY_LENGTH);
    }
    key->keygen(ikm, SECRET_KEY_LENGTH);
    auto wrapped = Napi::External<blst::SecretKey>::New(env, key);
    return constructor->New({wrapped});
}

Napi::Value SecretKey::FromBytes(const Napi::CallbackInfo &info)
{

    blst::SecretKey *key = new blst::SecretKey;
    Napi::Env env = info.Env();
    Napi::Value skBytes = info[0].As<Napi::Value>();
    if (!skBytes.IsTypedArray())
    {
        Napi::TypeError::New(env, "skBytes must be a Uint8Array").ThrowAsJavaScriptException();
    }
    auto skBytesArray = skBytes.As<Napi::TypedArrayOf<u_int8_t>>();
    auto skBytesData = skBytesArray.Data();
    auto skBytesLength = skBytesArray.ByteLength();
    if (skBytesLength != (size_t)SECRET_KEY_LENGTH)
    {
        Napi::Error::New(env, "skBytes must be 32 bytes long").ThrowAsJavaScriptException();
    }
    no_zero_bytes(skBytesData, skBytesLength);
    key->from_bendian(skBytesData);
    auto wrapped = Napi::External<blst::SecretKey>::New(env, key);
    return constructor->New({wrapped});
}

// Napi::Value SecretKey::ToPublicKey(const Napi::CallbackInfo &info)
// {
//     return info.Env().Undefined();
// }

// Napi::Value SecretKey::Sign(const Napi::CallbackInfo &info)
// {
//     return info.Env().Undefined();
// }

Napi::Value SecretKey::ToBytes(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    blst::byte bytesOut[SECRET_KEY_LENGTH] = {};
    key.to_bendian(bytesOut);
    Napi::TypedArrayOf<uint8_t> serialized = Napi::TypedArrayOf<uint8_t>::New(env, 32);
    for (int i = 0; i < SECRET_KEY_LENGTH; i++)
    {
        serialized[i] = bytesOut[i];
    }
    return serialized;
}