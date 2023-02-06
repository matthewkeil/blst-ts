#ifndef BLST_TS_SIGNATURE_H__
#define BLST_TS_SIGNATURE_H__

#include "napi.h"
#include "blst.hpp"
#include "addon.h"
#include "utils.h"

class Signature : public Napi::ObjectWrap<Signature>
{
public:
    std::unique_ptr<blst::P2> _point;
    std::unique_ptr<blst::P2_Affine> affine_;

    static void Init(const Napi::Env &env, Napi::Object &exports, Napi::Function &ctr, BlstTsAddon *module);
    static Napi::Value Deserialize(const Napi::CallbackInfo &info);
    Signature(const Napi::CallbackInfo &info);
    Napi::Value Serialize(const Napi::CallbackInfo &info);

private:
    BlstTsAddon &_module;
};

#endif /* BLST_TS_SIGNATURE_H__ */