#include <sstream>
#include <openssl/rand.h>
#include "napi.h"
#include "blst.hpp"
#include "addon.h"
#include "public_key.h"

void PublicKey::Init(const Napi::Env &env, Napi::Object &exports, Napi::Function &ctr, BlstTsAddon *module)
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
    ctr = DefineClass(env, "PublicKey", proto, module);
    exports.Set(Napi::String::New(env, "PublicKey"), ctr);
}

Napi::Value PublicKey::Deserialize(const Napi::CallbackInfo &info)
{
    BlstTsAddon *_module{reinterpret_cast<BlstTsAddon *>(info.Data())};
    Napi::Env env = info.Env();
    Napi::Value skBytes = info[0].As<Napi::Value>();
    if (!skBytes.IsTypedArray())
    {
        Napi::TypeError::New(env, "skBytes must be a Uint8Array").ThrowAsJavaScriptException();
    }
    auto skBytesArray = skBytes.As<Napi::TypedArrayOf<u_int8_t>>();
    auto skBytesData = skBytesArray.Data();
    auto skBytesLength = skBytesArray.ByteLength();
    if (skBytesLength != _module->_global_state->_secret_key_length)
    {
        std::ostringstream msg;
        msg << "BLST_ERROR::BLST_INVALID_SIZE - ikm must be " << _module->_global_state->_secret_key_length << " bytes long";
        Napi::Error::New(env, msg.str()).ThrowAsJavaScriptException();
        return;
    }

    auto external = Napi::External<void>::New(info.Env(), nullptr);
    Napi::Object wrapped = _module->secret_key_ctr_.New({external});
    PublicKey *sk = PublicKey::Unwrap(wrapped);
    sk->_key->from_bendian(skBytesData);
    return wrapped;
}

PublicKey::PublicKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<PublicKey>{info},
      _key{new blst::PublicKey},
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
    BlstTsAddon *_module{reinterpret_cast<BlstTsAddon *>(info.Data())};
    Napi::Env env = info.Env();
    Napi::Buffer<uint8_t> serialized = Napi::Buffer<uint8_t>::New(env, _module->_global_state->_secret_key_length);
    _key->to_bendian(serialized.Data());
    return serialized;
}
