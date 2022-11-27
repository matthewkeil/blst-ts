#ifndef __BLST_TS_PUBLIC_KEY_HPP__
#define __BLST_TS_PUBLIC_KEY_HPP__

#include <memory>
#include <typeinfo>
#include <iostream>
#include <napi.h>
#include "blst.hpp"
#include "blst_ts_utils.hpp"
#include "secret_key.hpp"

class PublicKey : public Napi::ObjectWrap<PublicKey>
{
public:
    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value Create(Napi::Env env, const blst::SecretKey *secretKey);
    static Napi::Value Create(Napi::Env env, blst::P1 *point, blst::P1_Affine *affine);
    static Napi::Value FromBytes(const Napi::CallbackInfo &info);
    PublicKey(const Napi::CallbackInfo &info);

    Napi::Value KeyValidate(const Napi::CallbackInfo &info);
    Napi::Value Serialize(const Napi::CallbackInfo &info, int length);
    Napi::Value Serialize(const Napi::CallbackInfo &info);
    Napi::Value Compress(const Napi::CallbackInfo &info);
    Napi::Value ToBytes(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    std::unique_ptr<blst::P1> point;
    std::unique_ptr<blst::P1_Affine> affine;
    bool is_point;
};

#endif /* __BLST_TS_PUBLIC_KEY_HPP__ */
