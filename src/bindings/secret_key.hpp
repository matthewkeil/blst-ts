#ifndef __BLST_TS_SECRET_KEY_HPP__
#define __BLST_TS_SECRET_KEY_HPP__

#include <memory>
#include <napi.h>
#include "blst.hpp"
#include "blst_ts_utils.hpp"
#include "public_key.hpp"
#include "signature.hpp"

class SecretKey : public Napi::ObjectWrap<SecretKey>
{

public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value Keygen(const Napi::CallbackInfo &info);
    static Napi::Value FromBytes(const Napi::CallbackInfo &info);
    SecretKey(const Napi::CallbackInfo &info);

    Napi::Value GetPublicKey(const Napi::CallbackInfo &info);
    Napi::Value Sign(const Napi::CallbackInfo &info);
    Napi::Value ToBytes(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    friend class PublicKey;
    std::unique_ptr<blst::SecretKey> key;
};

#endif /* __BLST_TS_SECRET_KEY_HPP__ */