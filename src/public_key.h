#ifndef BLST_TS_PUBLIC_KEY_H__
#define BLST_TS_PUBLIC_KEY_H__

#include <memory>
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

class PublicKeyArg
{
public:
    PublicKeyArg();
    PublicKeyArg(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg);
    blst::P1 *AsJacobian();
    blst::P1_Affine *AsAffine();

private:
    const BlstTsAddon *_addon;
    std::unique_ptr<blst::P1> _jacobian;
    std::unique_ptr<blst::P1_Affine> _affine;
    PublicKey *_public_key;
    Napi::Reference<Napi::Uint8Array> _bytes_ref;
    uint8_t *_bytes_data;
    size_t _bytes_length;
};

class PublicKeyArgArray : public std::vector<PublicKeyArg>
{
public:
    PublicKeyArgArray() : std::vector<PublicKeyArg>{} {};
    PublicKeyArgArray(const BlstTsAddon *module, const Napi::Env &env, const Napi::Value &raw_arg) : PublicKeyArgArray{}
    {
        if (!raw_arg.IsArray())
        {
            throw Napi::TypeError::New(env, "publicKeys argument must be of type PublicKeyArg[]");
        }
        Napi::Array arr = raw_arg.As<Napi::Array>();
        for (size_t i = 0; i < arr.Length(); i++)
        {
            (*this)[i] = std::move(PublicKeyArg{module, env, arr[i]});
        }
    }

    PublicKeyArg &operator[](size_t index)
    {
        return (*this)[index];
    }
};

#endif /* BLST_TS_PUBLIC_KEY_H__ */