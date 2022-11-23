#include "public_key.hpp"

Napi::FunctionReference *PublicKey::constructor;
Napi::Object PublicKey::Init(Napi::Env env, Napi::Object exports)
{
    // make a pointer to the constructor function so it can be bound to the
    // lifetime of the addon module
    Napi::Function publicKeyConstructor = DefineClass(env, "PublicKey",
                                                      {
                                                          StaticMethod("fromBytes", &PublicKey::FromBytes, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                          //   InstanceMethod("toPublicKey", &PublicKey::ToPublicKey, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          //   InstanceMethod("sign", &PublicKey::Sign, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("serialize", &PublicKey::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("toBytes", &PublicKey::ToBytes, static_cast<napi_property_attributes>(napi_enumerable)),
                                                      });
    constructor = new Napi::FunctionReference();
    *constructor = Napi::Persistent(publicKeyConstructor);
    exports.Set(Napi::String::New(env, "PublicKey"), publicKeyConstructor);
    return exports;
}

PublicKey::PublicKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<PublicKey>(info), env{info.Env()}, affine{nullptr}, point{nullptr}, is_affine{true}
{
    if (info[0].IsExternal() && info[1].IsExternal())
    {
        auto wrappedAffine = info[0].As<Napi::External<blst::P1_Affine>>();
        affine = wrappedAffine.Data();
        auto wrappedPoint = info[1].As<Napi::External<blst::P1>>();
        point = wrappedPoint.Data();
        if (point != nullptr)
        {
            is_affine = false;
        }
    }
    else if (info[0].IsObject())
    {
        // assume its a wrapped SecretKey
        auto secretKey = SecretKey::Unwrap(info[0].As<Napi::Object>());
        blst::P1 point{secretKey->key};
        blst::P1_Affine affine{point};
        is_affine = false;
    }
    else
    {
        Napi::Error::New(env, "new PublicKey() takes a SecretKey instance. Use static PublicKey.fromBytes()").ThrowAsJavaScriptException();
        return;
    }
}

PublicKey::~PublicKey()
{
    delete affine;
    if (!is_affine)
    {
        delete point;
    }
}

Napi::Value PublicKey::Create(Napi::Env env, blst::P1 *point, blst::P1_Affine *affine)
{
    auto wrappedAffine = Napi::External<blst::P1_Affine>::New(env, affine);
    auto wrappedPoint = Napi::External<blst::P1>::New(env, point);
    return constructor->New({wrappedAffine, wrappedPoint});
}

Napi::Value PublicKey::FromBytes(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    Napi::Value pkBytes = info[0].As<Napi::Value>();
    if (!pkBytes.IsTypedArray())
    {
        Napi::TypeError::New(env, "pkBytes must be a Uint8Array").ThrowAsJavaScriptException();
    }
    auto pkBytesArray = pkBytes.As<Napi::TypedArrayOf<u_int8_t>>();
    auto pkBytesData = pkBytesArray.Data();
    auto pkBytesLength = pkBytesArray.ByteLength();
    if ((pkBytesLength == 0 || pkBytesLength != (pkBytesData[0] & 0x80 ? PUBLIC_KEY_LENGTH_COMPRESSED : PUBLIC_KEY_LENGTH_UNCOMPRESSED)))
    {
        Napi::RangeError::New(env, "pkBytes must be 48 or 96 bytes long").ThrowAsJavaScriptException();
    }

    Napi::Value typeValue = info[1].As<Napi::Value>();
    if (!typeValue.IsNumber())
    {
        Napi::TypeError::New(env, "type must be of enum CoordType").ThrowAsJavaScriptException();
    }
    CoordType type = typeValue.As<Napi::Number>().Uint32Value() == (uint32_t)1 ? CoordType::Affine : CoordType::Jacobian;
    if ((type != CoordType::Jacobian) && (type != CoordType::Affine))
    {
        Napi::RangeError::New(env, "type must CoordType.Jacobian || CoordType.Affine").ThrowAsJavaScriptException();
    }
    blst::P1 *point = nullptr;
    blst::P1_Affine *affine = nullptr;
    if (type == CoordType::Jacobian)
    {
        point = new blst::P1{pkBytesData, pkBytesLength};
        affine = new blst::P1_Affine{*point};
    }
    else
    {
        affine = new blst::P1_Affine{pkBytesData, pkBytesLength};
    }

    return PublicKey::Create(env, point, affine);
}

// Napi::Value PublicKey::ToPublicKey(const Napi::CallbackInfo &info)
// {
//     return info.Env().Undefined();
// }

// Napi::Value PublicKey::Sign(const Napi::CallbackInfo &info)
// {
//     return info.Env().Undefined();
// }

Napi::Value PublicKey::Serialize(const Napi::CallbackInfo &info, int length)
{
    blst::byte out[length];
    if (is_affine)
    {
        if (length == PUBLIC_KEY_LENGTH_COMPRESSED)
        {
            printf("length %lu", sizeof(out));
            affine->compress(out);
        }
        else
            affine->serialize(out);
    }
    else
    {
        if (length == PUBLIC_KEY_LENGTH_COMPRESSED)
            point->compress(out);
        else
            point->serialize(out);
    }

    Napi::TypedArrayOf<uint8_t> serialized = Napi::TypedArrayOf<uint8_t>::New(env, length);
    for (int i = 0; i < length; i++)
    {
        serialized[i] = out[i];
    }
    return serialized;
}

Napi::Value PublicKey::Serialize(const Napi::CallbackInfo &info)
{
    return Serialize(info, PUBLIC_KEY_LENGTH_UNCOMPRESSED);
}

Napi::Value PublicKey::ToBytes(const Napi::CallbackInfo &info)
{
    return Serialize(info, PUBLIC_KEY_LENGTH_COMPRESSED);
}