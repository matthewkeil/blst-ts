#ifndef __BLST_TS_SIGNATURE_HPP__
#define __BLST_TS_SIGNATURE_HPP__

#include <napi.h>
#include "blst.hpp"
#include "blst_ts_utils.hpp"

class Signature : public Napi::ObjectWrap<Signature>
{
public:
    static const std::string kType_;

    static Napi::Object Init(Napi::Env env, Napi::Object exports);
    static Napi::Value FromBytes(const Napi::CallbackInfo &info);
    static Napi::Value Create(Napi::Env env, blst::P2 *point, blst::P2_Affine *affine);
    static Napi::Value FromMessage(const Napi::Env &env, const blst::byte *msg, size_t msg_length, const blst::SecretKey &key);

    Signature(const Napi::CallbackInfo &info);

    blst::P2_Affine AsAffine();
    Napi::Value SigValidate(const Napi::CallbackInfo &info);
    Napi::Value Serialize(const Napi::CallbackInfo &info, int length);
    Napi::Value Serialize(const Napi::CallbackInfo &info);
    Napi::Value Compress(const Napi::CallbackInfo &info);
    Napi::Value ToBytes(const Napi::CallbackInfo &info);

private:
    static Napi::FunctionReference constructor;
    std::unique_ptr<blst::P2_Affine> affine;
    std::unique_ptr<blst::P2> point;
    bool is_point;
};

#endif /* __BLST_TS_SIGNATURE_HPP__ */