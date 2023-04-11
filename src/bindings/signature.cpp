#include "signature.hpp"

const std::string Signature::kType_{"Signature"};
Napi::FunctionReference Signature::constructor;

Napi::Object Signature::Init(Napi::Env env, Napi::Object exports)
{
    // make a pointer to the constructor function so it can be bound to the
    // lifetime of the addon module
    Napi::Function signatureConstructor = DefineClass(env, "Signature",
                                                      {StaticMethod("fromBytes", &Signature::FromBytes, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
                                                       InstanceMethod("sigValidate", &Signature::SigValidate, static_cast<napi_property_attributes>(napi_enumerable)),
                                                       InstanceMethod("serialize", &Signature::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
                                                       InstanceMethod("compress", &Signature::Compress, static_cast<napi_property_attributes>(napi_enumerable)),
                                                       InstanceMethod("toBytes", &Signature::ToBytes, static_cast<napi_property_attributes>(napi_enumerable)),
                                                       InstanceValue("__type", Napi::String::New(env, SIGNATURE_TYPE), static_cast<napi_property_attributes>(napi_default))});
    constructor = Napi::Persistent(signatureConstructor);
    constructor.SuppressDestruct();
    exports.Set(Napi::String::New(env, "Signature"), signatureConstructor);
    return exports;
}

Signature::Signature(const Napi::CallbackInfo &info) : Napi::ObjectWrap<Signature>(info), affine{nullptr}, point{nullptr}, is_point{false}
{
    Napi::Env env = info.Env();
    if (info[0].IsExternal() && info[1].IsExternal())
    {
        auto wrappedAffine = info[0].As<Napi::External<blst::P2_Affine>>().Data();
        auto wrappedPoint = info[1].As<Napi::External<blst::P2>>().Data();
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

    Napi::Error::New(env, "new Signature() is a private constructor. Use static Signature.fromBytes()").ThrowAsJavaScriptException();
}

Napi::Value Signature::Create(Napi::Env env, blst::P2 *point, blst::P2_Affine *affine)
{
    auto wrappedAffine = Napi::External<blst::P2_Affine>::New(env, affine);
    auto wrappedPoint = Napi::External<blst::P2>::New(env, point);
    return constructor.New({wrappedAffine, wrappedPoint});
}

Napi::Value Signature::FromMessage(const Napi::Env &env, const blst::byte *msg, size_t msg_length, const blst::SecretKey &key)
{
    blst::P2 *point = new blst::P2;
    point->hash_to(msg, msg_length, DST);
    point->sign_with(key);
    return Signature::Create(env, point, nullptr);
}

Napi::Value Signature::FromBytes(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    Napi::Value sigBytes = info[0].As<Napi::Value>();
    if (!sigBytes.IsTypedArray())
    {
        Napi::TypeError::New(env, "sigBytes must be a Uint8Array or Buffer").ThrowAsJavaScriptException();
    }
    auto sigBytesArray = sigBytes.As<Napi::TypedArrayOf<u_int8_t>>();
    auto sigBytesData = sigBytesArray.Data();
    auto sigBytesLength = sigBytesArray.ByteLength();
    if (!(sigBytesLength == SIGNATURE_LENGTH_COMPRESSED || sigBytesLength == SIGNATURE_LENGTH_UNCOMPRESSED))
    {
        Napi::RangeError::New(env, "sigBytes must be 96 or 192 bytes long").ThrowAsJavaScriptException();
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

    // TODO: this may be a memory leak on BLST_ERROR. refactor or
    //       verify this is sound.
    blst::P2 *point = nullptr;
    blst::P2_Affine *affine = nullptr;
    try
    {
        if (type == CoordType::Jacobian)
        {
            point = new blst::P2{sigBytesData, sigBytesLength};
        }
        else
        {
            affine = new blst::P2_Affine{sigBytesData, sigBytesLength};
        }
    }
    catch (blst::BLST_ERROR err)
    {
        Napi::RangeError::New(env, get_blst_error_string(err)).ThrowAsJavaScriptException();
        return env.Undefined();
    }

    return Create(env, point, affine);
}

blst::P2_Affine Signature::AsAffine()
{
    if (is_point)
    {
        affine.reset(new blst::P2_Affine{point->to_affine()});
    }
    return blst::P2_Affine{*affine};
}

Napi::Value Signature::SigValidate(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();

    if (point != nullptr && !point->in_group())
    {
        Napi::Error::New(env, "blst::BLST_POINT_NOT_IN_GROUP").ThrowAsJavaScriptException();
    }
    else if (affine != nullptr && !affine->in_group())
    {
        Napi::Error::New(env, "blst::BLST_POINT_NOT_IN_GROUP").ThrowAsJavaScriptException();
    }
    return info.Env().Undefined();
}

Napi::Value Signature::Serialize(const Napi::CallbackInfo &info, int length)
{
    Napi::Env env = info.Env();
    blst::byte out[length];
    if (is_point)
    {
        if (length == SIGNATURE_LENGTH_COMPRESSED)
            point->compress(out);
        else
            point->serialize(out);
    }
    else
    {
        if (length == SIGNATURE_LENGTH_COMPRESSED)
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

Napi::Value Signature::Serialize(const Napi::CallbackInfo &info)
{
    return Serialize(info, SIGNATURE_LENGTH_UNCOMPRESSED);
}

Napi::Value Signature::Compress(const Napi::CallbackInfo &info)
{
    return Serialize(info, SIGNATURE_LENGTH_COMPRESSED);
}

Napi::Value Signature::ToBytes(const Napi::CallbackInfo &info)
{
    return Serialize(info, SIGNATURE_LENGTH_COMPRESSED);
}