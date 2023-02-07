#ifndef BLST_TS_SIGNATURE_H__
#define BLST_TS_SIGNATURE_H__

#include "napi.h"
#include "blst.hpp"
#include "addon.h"
#include "utils.h"

class Signature : public Napi::ObjectWrap<Signature>
{
public:
    bool _is_jacobian;
    std::unique_ptr<blst::P2> _jacobian;
    std::unique_ptr<blst::P2_Affine> _affine;

    static void Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *module);
    static Napi::Value Deserialize(const Napi::CallbackInfo &info);
    Signature(const Napi::CallbackInfo &info);
    Napi::Value Serialize(const Napi::CallbackInfo &info);
    Napi::Value SigValidate(const Napi::CallbackInfo &info);
    Napi::Value SigValidateSync(const Napi::CallbackInfo &info);

private:
    BlstTsAddon *_module;
};

#endif /* BLST_TS_SIGNATURE_H__ */