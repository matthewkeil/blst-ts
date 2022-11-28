#include "public_key.hpp"

Napi::FunctionReference PublicKey::constructor;

Napi::Object PublicKey::Init(Napi::Env env, Napi::Object exports)
{
    // make a pointer to the constructor function so it can be bound to the
    // lifetime of the addon module
    Napi::Function publicKeyConstructor = DefineClass(env, "PublicKey",
                                                      {
                                                          StaticMethod("fromBytes", &PublicKey::FromBytes, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                          InstanceMethod("keyValidate", &PublicKey::KeyValidate, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("serialize", &PublicKey::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("compress", &PublicKey::Compress, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceMethod("toBytes", &PublicKey::ToBytes, static_cast<napi_property_attributes>(napi_enumerable)),
                                                          InstanceValue("__type", Napi::String::New(env, PUBLIC_KEY_TYPE), static_cast<napi_property_attributes>(napi_default)),
                                                      });
    PublicKey::constructor = Napi::Persistent(publicKeyConstructor);
    // constructor.SuppressDestruct();
    exports.Set(Napi::String::New(env, "PublicKey"), publicKeyConstructor);
    return exports;
}

PublicKey::PublicKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<PublicKey>(info), point{nullptr}, affine{nullptr}, is_point{false}
{
    Napi::Env env = info.Env();
    if (info[0].IsExternal() && info[1].IsExternal())
    {
        auto wrappedAffine = info[0].As<Napi::External<blst::P1_Affine>>().Data();
        auto wrappedPoint = info[1].As<Napi::External<blst::P1>>().Data();
        if (wrappedAffine != nullptr)
        {
            affine.reset(wrappedAffine);
        }
        if (wrappedPoint != nullptr)
        {
            is_point = true;
            point.reset(wrappedPoint);
        }
        if (wrappedAffine == nullptr && wrappedPoint == nullptr)
        {
            Napi::TypeError::New(env, "Invalid internal creation. No point passed").ThrowAsJavaScriptException();
        }
        return;
    }
    else if (!info[0].IsObject())
    {
        Napi::Error::New(env, "new PublicKey() takes an object").ThrowAsJavaScriptException();
        return;
    }
    auto obj = info[0].As<Napi::Object>();
    if (!obj.Has("__type") || (obj.Get("__type").As<Napi::String>().Utf8Value().compare(SECRET_KEY_TYPE) != 0))
    {
        Napi::TypeError::New(env, "Must pass a SecretKey to new PublicKey()").ThrowAsJavaScriptException();
        return;
    }
    auto secretKey = SecretKey::Unwrap(obj);
    point.reset(new blst::P1{*(secretKey->key)});
    is_point = true;
}

Napi::Value PublicKey::Create(Napi::Env env, const blst::SecretKey *secretKey)
{
    // This object must flow into the unique_ptr in the constructor or it
    // will leak.  Stay vigilant!!
    blst::P1 *point{new blst::P1{*secretKey}};
    return Create(env, point, nullptr);
}

Napi::Value PublicKey::Create(Napi::Env env, blst::P1 *point, blst::P1_Affine *affine)
{
    auto wrappedAffine = Napi::External<blst::P1_Affine>::New(env, affine);
    auto wrappedPoint = Napi::External<blst::P1>::New(env, point);
    return constructor.New({wrappedAffine, wrappedPoint});
}

Napi::Value PublicKey::FromBytes(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    Napi::Value bytes = info[0].As<Napi::Value>();
    if (!bytes.IsTypedArray())
    {
        Napi::TypeError::New(env, "pkBytes must be a Uint8Array or Buffer").ThrowAsJavaScriptException();
    }
    auto bytesArray = bytes.As<Napi::TypedArrayOf<u_int8_t>>();
    auto bytesData = bytesArray.Data();
    auto bytesLength = bytesArray.ByteLength();
    if (!(bytesLength == PUBLIC_KEY_LENGTH_COMPRESSED || bytesLength == PUBLIC_KEY_LENGTH_UNCOMPRESSED))
    {
        Napi::RangeError::New(env, "pkBytes must be 48 or 96 bytes long").ThrowAsJavaScriptException();
        return env.Undefined();
    }

    auto type = CoordType::Jacobian;
    if (!info[1].IsUndefined())
    {
        Napi::Value typeValue = info[1].As<Napi::Value>();
        if (!typeValue.IsNumber())
        {
            Napi::TypeError::New(env, "type must be of enum CoordType").ThrowAsJavaScriptException();
        }
        if (typeValue.As<Napi::Number>().Uint32Value() == (uint32_t)0)
        {
            type = CoordType::Affine;
        }
    }

    blst::P1 *point = nullptr;
    blst::P1_Affine *affine = nullptr;
    try
    {
        if (type == CoordType::Jacobian)
        {
            point = new blst::P1{bytesData, bytesLength};
        }
        else
        {
            affine = new blst::P1_Affine{bytesData, bytesLength};
        }
    }
    catch (blst::BLST_ERROR err)
    {
        Napi::RangeError::New(env, get_blst_error_string(err)).ThrowAsJavaScriptException();
        return env.Undefined();
    }

    return PublicKey::Create(env, point, affine);
}

Napi::Value PublicKey::KeyValidate(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    if (is_point)
    {
        if (point->is_inf())
        {
            Napi::Error::New(env, "blst::BLST_PK_IS_INFINITY").ThrowAsJavaScriptException();
        }
        if (!point->in_group())
        {
            Napi::Error::New(env, "blst::BLST_POINT_NOT_IN_GROUP").ThrowAsJavaScriptException();
        }
    }
    else
    {
        if (affine->is_inf())
        {
            Napi::Error::New(env, "blst::BLST_PK_IS_INFINITY").ThrowAsJavaScriptException();
        }
        if (!affine->in_group())
        {
            Napi::Error::New(env, "blst::BLST_POINT_NOT_IN_GROUP").ThrowAsJavaScriptException();
        }
    }
    return info.Env().Undefined();
}

Napi::Value PublicKey::Serialize(const Napi::CallbackInfo &info, int length)
{
    Napi::Env env = info.Env();
    blst::byte out[length];
    if (is_point)
    {
        if (length == PUBLIC_KEY_LENGTH_COMPRESSED)
            point->compress(out);
        else
            point->serialize(out);
    }
    else
    {
        if (length == PUBLIC_KEY_LENGTH_COMPRESSED)
            affine->compress(out);
        else
            affine->serialize(out);
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

Napi::Value PublicKey::Compress(const Napi::CallbackInfo &info)
{
    return Serialize(info, PUBLIC_KEY_LENGTH_COMPRESSED);
}

Napi::Value PublicKey::ToBytes(const Napi::CallbackInfo &info)
{
    return Serialize(info, PUBLIC_KEY_LENGTH_COMPRESSED);
}