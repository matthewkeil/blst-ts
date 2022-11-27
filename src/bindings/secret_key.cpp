#include "secret_key.hpp"

Napi::FunctionReference SecretKey::constructor;

Napi::Object SecretKey::Init(Napi::Env env, Napi::Object exports)
{
    // make a pointer to the constructor function so it can be bound to the
    // lifetime of the addon module
    Napi::Function secretKeyConstructor = DefineClass(env, "SecretKey",
                                                      {
                                                          StaticMethod("keygen", &SecretKey::Keygen, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                          StaticMethod("fromBytes", &SecretKey::FromBytes, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                          InstanceMethod("getPublicKey", &SecretKey::GetPublicKey, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("sign", &SecretKey::Sign, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("toBytes", &SecretKey::ToBytes, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceValue("__type", Napi::String::New(env, SECRET_KEY_TYPE), static_cast<napi_property_attributes>(napi_default)),
                                                      });
    constructor = Napi::Persistent(secretKeyConstructor);
    constructor.SuppressDestruct();
    exports.Set(Napi::String::New(env, "SecretKey"), secretKeyConstructor);
    return exports;
}

SecretKey::SecretKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<SecretKey>(info), key{nullptr}
{
    Napi::Env env = info.Env();
    if (info[0].IsExternal())
    {
        auto passedKey = info[0].As<Napi::External<blst::SecretKey>>();
        // passed as a naked pointer so make sure this gets put in the
        // unique_ptr to manage the lifetime of the blst::SecretKey
        key.reset(passedKey.Data());
        return;
    }
    else if (info.Length() > 0)
    {
        Napi::Error::New(env, "No arguments are allowed in SecretKey constructor").ThrowAsJavaScriptException();
        return;
    }
    // if no blst::SecretKey was wrapped and passed to constructor
    // then new up the SecretKey in in the context of the unique_ptr
    // so it is cleaned up in the destructor
    key.reset(new blst::SecretKey);
    blst::byte random[SECRET_KEY_LENGTH] = {};
    random_bytes_non_zero(random, SECRET_KEY_LENGTH);
    key->keygen(random, SECRET_KEY_LENGTH);
}

Napi::Value SecretKey::Keygen(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    auto ikm = std::make_unique<blst::byte[]>(32);
    Napi::Value entropyVal = info[0].As<Napi::Value>();

    if (entropyVal.IsUndefined()) // no entropy passed
    {
        random_bytes_non_zero(ikm.get(), SECRET_KEY_LENGTH);
    }
    else if (!entropyVal.IsTypedArray())
    {
        Napi::Error err = Napi::Error::New(env, "IKM for new SecretKey(ikm) must be a Uint8Array");
        err.ThrowAsJavaScriptException();
    }
    else
    {
        auto entropyArray = entropyVal.As<Napi::TypedArrayOf<u_int8_t>>();
        if (entropyArray.ByteLength() != SECRET_KEY_LENGTH)
        {
            Napi::Error err = Napi::Error::New(env, "ikm must be 32 bytes long");
            err.ThrowAsJavaScriptException();
        }
        for (int i = 0; i < SECRET_KEY_LENGTH; i++)
        {
            ikm.get()[i] = entropyArray[i];
        }
    }

    // This pointer will be passed into a unique_ptr in constructor.
    // unique_ptr will manage the lifetime of this object and delete
    // in the destructor.
    blst::SecretKey *key = new blst::SecretKey;
    key->keygen(ikm.get(), SECRET_KEY_LENGTH);
    auto wrapped = Napi::External<blst::SecretKey>::New(env, key);
    return constructor.New({wrapped});
}

Napi::Value SecretKey::FromBytes(const Napi::CallbackInfo &info)
{
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
    // This pointer will be passed into a unique_ptr in constructor.
    // unique_ptr will manage the lifetime of this object and delete
    // in the destructor.
    blst::SecretKey *key = new blst::SecretKey;
    key->from_bendian(skBytesData);
    auto wrapped = Napi::External<blst::SecretKey>::New(env, key);
    return constructor.New({wrapped});
}

Napi::Value SecretKey::GetPublicKey(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    return PublicKey::Create(env, key.get());
}

Napi::Value SecretKey::Sign(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    Napi::Value msgValue = info[0];
    if (!msgValue.IsTypedArray())
    {
        Napi::TypeError::New(env, "msg to sign must be a Uint8Array or Buffer").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    auto msgArray = msgValue.As<Napi::TypedArrayOf<uint8_t>>();
    blst::byte *msg = msgArray.Data();
    size_t msgLength = msgArray.ByteLength();
    return Signature::FromMessage(env, msg, msgLength, *key);
}

Napi::Value SecretKey::ToBytes(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    blst::byte bytesOut[SECRET_KEY_LENGTH] = {};
    key->to_bendian(bytesOut);
    Napi::TypedArrayOf<uint8_t> serialized = Napi::TypedArrayOf<uint8_t>::New(env, 32);
    for (int i = 0; i < SECRET_KEY_LENGTH; i++)
    {
        serialized[i] = bytesOut[i];
    }
    return serialized;
}