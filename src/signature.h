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

class SignatureArg
{
public:
    SignatureArg(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg)
        : _addon{addon},
          _signature{nullptr},
          _bytes_ref{},
          _bytes_data{nullptr},
          _bytes_length{0}
    {
        if (raw_arg.IsTypedArray() && raw_arg.As<Napi::TypedArray>().TypedArrayType() == napi_uint8_array)
        {
            _bytes_ref = Napi::Persistent(raw_arg.As<Napi::Uint8Array>());
            _bytes_data = _bytes_ref.Value().Data();
            _bytes_length = _bytes_ref.Value().ByteLength();
            return;
        }
        if (raw_arg.IsObject())
        {
            Napi::Object raw_obj = raw_arg.As<Napi::Object>();
            if (!raw_obj.Has("__type") || (raw_obj.Get("__type").As<Napi::String>().Utf8Value().compare(_addon->_global_state->_signature_type) != 0))
            {
                _signature = Signature::Unwrap(raw_obj);
                return;
            }
        }
        throw Napi::TypeError::New(env, "SignatureArg must be a Signature instance or a serialized Uint8Array");
    };

    SignatureArg(SignatureArg &&source) = default;
    SignatureArg &operator=(SignatureArg &&source) = default;
    SignatureArg(const SignatureArg &source) = delete;
    SignatureArg &operator=(const SignatureArg &source) = delete;

    const blst::P2 *AsJacobian()
    {
        if (_signature)
        {
            if (!_signature->_is_jacobian)
            {
                _signature->_jacobian.reset(new blst::P2{_signature->_affine->to_jacobian()});
                _signature->_is_jacobian = true;
            }
            return _signature->_jacobian.get();
        }
        if (_jacobian.get() == nullptr)
        {
            _jacobian.reset(new blst::P2{_bytes_data, _bytes_length});
        }
        return _jacobian.get();
    }

    const blst::P2_Affine *AsAffine()
    {
        if (_signature)
        {
            // need to check this works to not duplicate if affine is already build
            if (_signature->_is_jacobian /* && _signature->_jacobian->is_inf() */)
            {
                _signature->_affine.reset(new blst::P2_Affine{_signature->_jacobian->to_affine()});
            }
            return _signature->_affine.get();
        }
        if (_affine.get() == nullptr)
        {
            _affine.reset(new blst::P2_Affine{_bytes_data, _bytes_length});
        }
        return _affine.get();
    }

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
    SignatureArgArray(const BlstTsAddon *module, const Napi::Env &env, const Napi::Value &raw_arg) : SignatureArgArray{}
    {
        if (!raw_arg.IsArray())
        {
            throw Napi::TypeError::New(env, "publicKeys argument must be of type SignatureArg[]");
        }
        Napi::Array arr = raw_arg.As<Napi::Array>();
        for (size_t i = 0; i < arr.Length(); i++)
        {
            (*this)[i] = std::move(SignatureArg{module, env, arr[i]});
        }
    }

    SignatureArgArray(SignatureArgArray &&source) = default;
    SignatureArgArray &operator=(SignatureArgArray &&source) = default;
    SignatureArgArray(const SignatureArgArray &source) = delete;
    SignatureArgArray &operator=(const SignatureArgArray &source) = delete;

    SignatureArg &operator[](size_t index)
    {
        return (*this)[index];
    }
};

#endif /* BLST_TS_SIGNATURE_H__ */