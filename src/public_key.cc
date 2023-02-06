#include "public_key.h"

void PublicKey::Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *module)
{
    auto proto = {
        StaticMethod("deserialize", &PublicKey::Deserialize, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        InstanceMethod("serialize", &PublicKey::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
        /**
         * This should be switched to the resolution of this ticket.
         * https://github.com/nodejs/node-addon-api/issues/1260
         *
         * Until then query through JS to make sure the object passed through from JS
         * is the correct type to prevent seg fault
         */
        InstanceValue("__type", Napi::String::New(env, module->_global_state->public_key_type_), static_cast<napi_property_attributes>(napi_default)),
    };
    Napi::Function ctr = DefineClass(env, "PublicKey", proto, module);
    module->_public_key_ctr = Napi::Persistent(ctr);
    exports.Set(Napi::String::New(env, "PublicKey"), ctr);
}

Napi::Value PublicKey::Deserialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    BlstTsAddon *module = env.GetInstanceData<BlstTsAddon>();
    UINT8_ARG_CHECK_2_LENGTHS_UNDEFINED(
        info,
        env,
        0,
        pkBytes,
        module->_global_state->_public_key_compressed_length,
        module->_global_state->_public_key_uncompressed_length,
        'pkBytes')
    Napi::Object wrapped = module->_secret_key_ctr.New({Napi::External<void>::New(env, nullptr)});
    PublicKey *pk = PublicKey::Unwrap(wrapped);
    pk->_is_jacobian = true;
    if (!info[1].IsUndefined())
    {
        Napi::Value type_val = info[1].As<Napi::Value>();
        if (!type_val.IsNumber())
        {
            Napi::TypeError::New(env, "type must be of enum CoordType (number)").ThrowAsJavaScriptException();
            return env.Undefined();
        }
        if (type_val.As<Napi::Number>().Uint32Value() == 0)
        {
            pk->_is_jacobian = false;
        }
    }
    try
    {
        if (pk->_is_jacobian)
        {
            pk->_jacobian.reset(new blst::P1{pkBytes.Data(), pkBytes.ByteLength()});
        }
        else
        {
            pk->_affine.reset(new blst::P1_Affine{pkBytes.Data(), pkBytes.ByteLength()});
        }
    }
    catch (blst::BLST_ERROR err)
    {
        Napi::RangeError::New(env, module->GetBlstErrorString(err)).ThrowAsJavaScriptException();
        return env.Undefined();
    }
    return wrapped;
}

PublicKey::PublicKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<PublicKey>{info},
      _is_jacobian{false},
      _jacobian{nullptr},
      _affine{nullptr},
      _module{*reinterpret_cast<BlstTsAddon *>(info.Data())}
{
    Napi::Env env = info.Env();
    if (!info[0].IsExternal())
    {
        Napi::Error::New(env, "PublicKey constructor is private").ThrowAsJavaScriptException();
        return;
    }
};

Napi::Value PublicKey::Serialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    bool compressed{true};
    if (!info[1].IsUndefined())
    {
        compressed = info[1].ToBoolean().Value();
    }
    size_t length = compressed ? _module._global_state->_public_key_compressed_length : _module._global_state->_public_key_uncompressed_length;
    Napi::Buffer<uint8_t> serialized = Napi::Buffer<uint8_t>::New(env, length);
    if (compressed)
    {
        if (_is_jacobian)
            _jacobian->compress(serialized.Data());
        else
            _affine->compress(serialized.Data());
    }
    else
    {
        if (_is_jacobian)
            _jacobian->serialize(serialized.Data());
        else
            _affine->serialize(serialized.Data());
    }
    return serialized;
}
