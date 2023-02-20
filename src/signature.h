#ifndef BLST_TS_SIGNATURE_H__
#define BLST_TS_SIGNATURE_H__

#include <memory>
#include "napi.h"
#include "blst.hpp"
#include "addon.h"

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

class SignatureArg
{
public:
    SignatureArg(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg);

    SignatureArg(SignatureArg &&source) = default;
    SignatureArg &operator=(SignatureArg &&source) = default;
    SignatureArg(const SignatureArg &source) = delete;
    SignatureArg &operator=(const SignatureArg &source) = delete;

    const blst::P2 *AsJacobian();

    const blst::P2_Affine *AsAffine();

private:
    const BlstTsAddon *_addon;
    std::unique_ptr<blst::P2> _jacobian;
    std::unique_ptr<blst::P2_Affine> _affine;
    Signature *_signature;
    Napi::Reference<Napi::Uint8Array> _bytes_ref;
    uint8_t *_bytes_data;
    size_t _bytes_length;
};

class SignatureArgArray : public std::vector<SignatureArg>
{
public:
    SignatureArgArray() : std::vector<SignatureArg>{} {};
    SignatureArgArray(const BlstTsAddon *module, const Napi::Env &env, const Napi::Value &raw_arg);

    SignatureArgArray(SignatureArgArray &&source) = default;
    SignatureArgArray &operator=(SignatureArgArray &&source) = default;
    SignatureArgArray(const SignatureArgArray &source) = delete;
    SignatureArgArray &operator=(const SignatureArgArray &source) = delete;
};

#endif /* BLST_TS_SIGNATURE_H__ */