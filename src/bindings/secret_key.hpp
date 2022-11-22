#pragma once

#include <napi.h>
#include "blst.hpp"
#include "blst_ts_utils.hpp"

class SecretKey : public Napi::ObjectWrap<SecretKey>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value Keygen(const Napi::CallbackInfo &info);
    static Napi::Value FromBytes(const Napi::CallbackInfo &info);
    SecretKey(const Napi::CallbackInfo &info);

    // Napi::Value ToPublicKey(const Napi::CallbackInfo &info);
    // Napi::Value Sign(const Napi::CallbackInfo &info);
    // Napi::Value ToBytes(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference *constructor;
    blst::SecretKey key;
    Napi::Env env;
};