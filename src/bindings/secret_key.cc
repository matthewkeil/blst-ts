#include "napi.h"
#include "blst.hpp"
#include "addon.h"
#include "secret_key.h"

void SecretKey::Init(const Napi::Env &env, Napi::Object &exports, Napi::Function &ctr, BlstTsAddon *module)
{
    auto proto = {
        StaticMethod("fromKeygen", &SecretKey::FromKeygen, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        StaticMethod("fromKeygenSync", &SecretKey::FromKeygenSync, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        StaticMethod("deserialize", &SecretKey::Deserialize, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        InstanceMethod("serialize", &SecretKey::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("toPublicKey", &SecretKey::ToPublicKey, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("toPublicKeySync", &SecretKey::ToPublicKeySync, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("sign", &SecretKey::Sign, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("signSync", &SecretKey::SignSync, static_cast<napi_property_attributes>(napi_enumerable)),
        /**
         * This should be switched to the resolution of this ticket.
         * https://github.com/nodejs/node-addon-api/issues/1260
         *
         * Until then query through JS to make sure the object passed through from JS
         * is the correct type to prevent seg fault
         */
        InstanceValue("__type", Napi::String::New(env, module->global_state_->secret_key_type_), static_cast<napi_property_attributes>(napi_default)),
    };
    ctr = DefineClass(env, "SecretKey", proto, module);
    exports.Set(Napi::String::New(env, "SecretKey"), ctr);
}

Napi::Value SecretKey::FromKeygen(const Napi::CallbackInfo &info)
{
}

Napi::Value SecretKey::FromKeygenSync(const Napi::CallbackInfo &info)
{
}

Napi::Value SecretKey::Deserialize(const Napi::CallbackInfo &info)
{
}

SecretKey::SecretKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<SecretKey>{info},
      module_{*reinterpret_cast<BlstTsAddon *>(info.Data())} {};

Napi::Value SecretKey::Serialize(const Napi::CallbackInfo &info)
{
}

Napi::Value SecretKey::ToPublicKey(const Napi::CallbackInfo &info)
{
}

Napi::Value SecretKey::ToPublicKeySync(const Napi::CallbackInfo &info)
{
}

Napi::Value SecretKey::Sign(const Napi::CallbackInfo &info)
{
}

Napi::Value SecretKey::SignSync(const Napi::CallbackInfo &info)
{
}