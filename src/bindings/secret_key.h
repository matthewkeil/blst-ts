#ifndef BLST_TS_SECRET_KEY_H__
#define BLST_TS_SECRET_KEY_H__

#include "napi.h"

class SecretKey : public Napi::ObjectWrap<SecretKey>
{
public:
    static void Init(const Napi::Env &env, Napi::Object &exports, Napi::Function &ctr, BlstTsAddon *module);
    static Napi::Value FromKeygen(const Napi::CallbackInfo &info);
    static Napi::Value FromKeygenSync(const Napi::CallbackInfo &info);
    static Napi::Value Deserialize(const Napi::CallbackInfo &info);
    SecretKey(const Napi::CallbackInfo &info);
    Napi::Value Serialize(const Napi::CallbackInfo &info);
    Napi::Value ToPublicKey(const Napi::CallbackInfo &info);
    Napi::Value ToPublicKeySync(const Napi::CallbackInfo &info);
    Napi::Value Sign(const Napi::CallbackInfo &info);
    Napi::Value SignSync(const Napi::CallbackInfo &info);

private:
    BlstTsAddon &module_;
};

#endif /* BLST_TS_SECRET_KEY_H__ */