#include "signature.hpp"

Napi::FunctionReference *Signature::constructor;
Napi::Object Signature::Init(Napi::Env env, Napi::Object exports)
{
    // make a pointer to the constructor function so it can be bound to the
    // lifetime of the addon module
    Napi::Function signatureConstructor = DefineClass(env, "Signature",
                                                      {
                                                          StaticMethod("fromBytes", &Signature::FromBytes, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                          //   InstanceMethod("toPublicKey", &Signature::ToPublicKey, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          //   InstanceMethod("sign", &Signature::Sign, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          //   InstanceMethod("toBytes", &Signature::ToBytes, static_cast<napi_property_attributes>(napi_enumerable)),
                                                      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(signatureConstructor);
    exports.Set(Napi::String::New(env, "Signature"), signatureConstructor);
    return exports;
}

Signature::Signature(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Signature>(info), env{info.Env()}, affine{}, point{}
{
    if (info[0].IsExternal())
    {
        // auto passedKey = info[0].As<Napi::External<blst::SecretKey>>();
        // key = *passedKey.Data();
        return;
    }
    else if (info.Length() > 0)
    {
        Napi::Error::New(env, "No arguments are allowed in Signature constructor").ThrowAsJavaScriptException();
        return;
    }
}

Napi::Value Signature::FromBytes(const Napi::CallbackInfo &info)
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

// Napi::Value Signature::ToBytes(const Napi::CallbackInfo &info)
// {
//     auto env = info.Env();
//     blst::byte bytesOut[SECRET_KEY_LENGTH] = {};
//     key.to_bendian(bytesOut);
//     Napi::TypedArrayOf<uint8_t> serialized = Napi::TypedArrayOf<uint8_t>::New(env, 32);
//     for (int i = 0; i < SECRET_KEY_LENGTH; i++)
//     {
//         serialized[i] = bytesOut[i];
//     }
//     return serialized;
// }