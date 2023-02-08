#ifndef BLST_TS_PUBLIC_KEY_H__
#define BLST_TS_PUBLIC_KEY_H__

#include "napi.h"
#include "blst.hpp"
#include "addon.h"
#include "utils.h"

class PublicKey : public Napi::ObjectWrap<PublicKey>
{
public:
    bool _is_jacobian;
    std::unique_ptr<blst::P1> _jacobian;
    std::unique_ptr<blst::P1_Affine> _affine;

    static void Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *module);
    static Napi::Value Deserialize(const Napi::CallbackInfo &info);
    PublicKey(const Napi::CallbackInfo &info);
    Napi::Value Serialize(const Napi::CallbackInfo &info);
    Napi::Value KeyValidate(const Napi::CallbackInfo &info);
    Napi::Value KeyValidateSync(const Napi::CallbackInfo &info);

private:
    BlstTsAddon &_module;
};

#endif /* BLST_TS_PUBLIC_KEY_H__ */