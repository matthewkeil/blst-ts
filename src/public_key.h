#ifndef BLST_TS_PUBLIC_KEY_H__
#define BLST_TS_PUBLIC_KEY_H__

#include <memory>
#include "napi.h"
#include "blst.hpp"
#include "addon.h"

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

class PublicKeyArg
{
public:
    PublicKeyArg(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg);

    PublicKeyArg &operator=(const PublicKeyArg &source) = delete;
    PublicKeyArg(const PublicKeyArg &source) = delete;
    PublicKeyArg &operator=(PublicKeyArg &&source) = default;
    PublicKeyArg(PublicKeyArg &&source) = default;

    blst::P1 *AsJacobian();
    blst::P1_Affine *AsAffine();

private:
    const BlstTsAddon *_addon;
    std::unique_ptr<blst::P1> _jacobian;
    std::unique_ptr<blst::P1_Affine> _affine;
    PublicKey *_public_key;
    Uint8ArrayArg _bytes;
};

class PublicKeyArgArray
{
public:
    PublicKeyArgArray(
        const BlstTsAddon *module,
        const Napi::Env &env,
        const Napi::Value &raw_arg);
    PublicKeyArgArray(
        const BlstTsAddon *module,
        const Napi::CallbackInfo &info,
        const size_t arg_position)
        : PublicKeyArgArray{module, info.Env(), info[arg_position]} {}

    PublicKeyArgArray &operator=(const PublicKeyArgArray &source) = delete;
    PublicKeyArgArray(const PublicKeyArgArray &source) = delete;
    PublicKeyArgArray &operator=(PublicKeyArgArray &&source) = default;
    PublicKeyArgArray(PublicKeyArgArray &&source) = default;
    PublicKeyArg &operator[](size_t index)
    {
        return _keys[index];
    }

    size_t Size() { return _keys.size(); }
    void Reserve(size_t size) { return _keys.reserve(size); }

private:
    std::vector<PublicKeyArg> _keys;
};

#endif /* BLST_TS_PUBLIC_KEY_H__ */