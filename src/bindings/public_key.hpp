#pragma once

#include <memory>
#include <napi.h>
#include "blst.hpp"
#include "blst_ts_utils.hpp"
#include "secret_key.hpp"

class PublicKey : public Napi::ObjectWrap<PublicKey>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value Create(Napi::Env env, blst::P1 *point, blst::P1_Affine *affine);
    static Napi::Value Create(Napi::Env env, blst::SecretKey *secretKey);
    // static Napi::Value Keygen(const Napi::CallbackInfo &info);
    static Napi::Value FromBytes(const Napi::CallbackInfo &info);
    PublicKey(const Napi::CallbackInfo &info);

    Napi::Value ToPublicKey(const Napi::CallbackInfo &info);
    Napi::Value Sign(const Napi::CallbackInfo &info);
    Napi::Value Serialize(const Napi::CallbackInfo &info, int length);
    Napi::Value Serialize(const Napi::CallbackInfo &info);
    Napi::Value ToBytes(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference *constructor;
    Napi::Env env;
    std::unique_ptr<blst::P1_Affine> affine;
    std::unique_ptr<blst::P1> point;
    bool is_affine;
};