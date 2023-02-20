#include "public_key.h"

void PublicKey::Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *module)
{
    auto proto = {
        StaticMethod("deserialize", &PublicKey::Deserialize, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        InstanceMethod("serialize", &PublicKey::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("keyValidate", &PublicKey::KeyValidate, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("keyValidateSync", &PublicKey::KeyValidateSync, static_cast<napi_property_attributes>(napi_enumerable)),
        /**
         * This should be switched to the resolution of this ticket.
         * https://github.com/nodejs/node-addon-api/issues/1260
         *
         * Until then query through JS to make sure the object passed through from JS
         * is the correct type to prevent seg fault
         */
        InstanceValue("__type", Napi::String::New(env, module->_global_state->_public_key_type), static_cast<napi_property_attributes>(napi_default)),
    };
    Napi::Function ctr = DefineClass(env, "PublicKey", proto, module);
    module->_public_key_ctr = Napi::Persistent(ctr);
    exports.Set(Napi::String::New(env, "PublicKey"), ctr);
}

/**
 *
 *
 * PublicKey Workers
 *
 *
 */
namespace
{
    class KeyValidateWorker : public BlstAsyncWorker
    {
    public:
        KeyValidateWorker(
            const Napi::CallbackInfo &info, bool is_jacobian, blst::P1 &jacobian, blst::P1_Affine &affine)
            : BlstAsyncWorker(info),
              _is_jacobian{true},
              _jacobian{jacobian},
              _affine{affine} {};

    protected:
        void Setup() override{};

        void Execute() override
        {
            if (_is_jacobian)
            {
                if (_jacobian.is_inf())
                {
                    SetError("blst::BLST_PK_IS_INFINITY");
                }
                if (!_jacobian.in_group())
                {
                    SetError("blst::BLST_POINT_NOT_IN_GROUP");
                }
            }
            else
            {
                if (_affine.is_inf())
                {
                    SetError("blst::BLST_PK_IS_INFINITY");
                }
                if (!_affine.in_group())
                {
                    SetError("blst::BLST_POINT_NOT_IN_GROUP");
                }
            }
        };

        Napi::Value GetReturnValue() override
        {
            return _env.Undefined();
        };

    private:
        bool _is_jacobian;
        blst::P1 &_jacobian;
        blst::P1_Affine &_affine;
    };
}

/**
 *
 *
 * PublicKey Methods
 *
 *
 */
Napi::Value PublicKey::Deserialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    BlstTsAddon *module = env.GetInstanceData<BlstTsAddon>();
    Napi::Object wrapped = module->_public_key_ctr.New({Napi::External<void>::New(env, nullptr)});
    PublicKey *pk = PublicKey::Unwrap(wrapped);
    pk->_is_jacobian = true;
    if (!info[1].IsUndefined())
    {
        Napi::Value type_val = info[1].As<Napi::Value>();
        if (!type_val.IsNumber())
        {
            Napi::TypeError::New(env, "type must be of enum CoordType (number)").ThrowAsJavaScriptException();
            return env.Undefined();
        }
        if (type_val.As<Napi::Number>().Uint32Value() == 0)
        {
            pk->_is_jacobian = false;
        }
    }

    Uint8ArrayArg pk_bytes{
        info,
        0,
        "pkBytes"};
    pk_bytes.ValidateLength(module->_global_state->_public_key_compressed_length, module->_global_state->_public_key_uncompressed_length);
    if (pk_bytes.HasError())
    {
        pk_bytes.ThrowJsException();
        return env.Undefined();
    }
    try
    {
        if (pk->_is_jacobian)
        {
            pk->_jacobian.reset(new blst::P1{pk_bytes.Data(), pk_bytes.ByteLength()});
        }
        else
        {
            pk->_affine.reset(new blst::P1_Affine{pk_bytes.Data(), pk_bytes.ByteLength()});
        }
    }
    catch (blst::BLST_ERROR err)
    {
        Napi::RangeError::New(env, module->GetBlstErrorString(err)).ThrowAsJavaScriptException();
        return env.Undefined();
    }
    return wrapped;
}

PublicKey::PublicKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<PublicKey>{info},
      _is_jacobian{false},
      _jacobian{nullptr},
      _affine{nullptr},
      _module{*reinterpret_cast<BlstTsAddon *>(info.Data())}
{
    Napi::Env env = info.Env();
    if (!info[0].IsExternal())
    {
        Napi::Error::New(env, "PublicKey constructor is private").ThrowAsJavaScriptException();
        return;
    }
};

Napi::Value PublicKey::Serialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    bool compressed{true};
    if (!info[0].IsUndefined())
    {
        compressed = info[0].ToBoolean().Value();
    }
    size_t length = compressed ? _module._global_state->_public_key_compressed_length : _module._global_state->_public_key_uncompressed_length;
    Napi::Buffer<uint8_t> serialized = Napi::Buffer<uint8_t>::New(env, length);
    if (compressed)
    {
        if (_is_jacobian)
            _jacobian->compress(serialized.Data());
        else
            _affine->compress(serialized.Data());
    }
    else
    {
        if (_is_jacobian)
            _jacobian->serialize(serialized.Data());
        else
            _affine->serialize(serialized.Data());
    }
    return serialized;
}

Napi::Value PublicKey::KeyValidate(const Napi::CallbackInfo &info)
{
    KeyValidateWorker *worker = new KeyValidateWorker{info, _is_jacobian, *_jacobian, *_affine};
    return worker->Run();
}

Napi::Value PublicKey::KeyValidateSync(const Napi::CallbackInfo &info)
{
    KeyValidateWorker worker{info, _is_jacobian, *_jacobian, *_affine};
    return worker.RunSync();
}

/**
 *
 *
 * PublicKeyArg
 *
 *
 */
PublicKeyArg::PublicKeyArg(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg)
    : _addon{addon},
      _jacobian{new blst::P1()},
      _affine{new blst::P1_Affine()},
      _public_key{nullptr},
      _bytes{env}
{
    if (raw_arg.IsObject())
    {
        Napi::Object raw_obj = raw_arg.As<Napi::Object>();
        if (raw_obj.Has("__type") &&
            !raw_obj
                 .Get("__type")
                 .As<Napi::String>()
                 .Utf8Value()
                 .compare(_addon->_global_state->_public_key_type))
        {
            _public_key = PublicKey::Unwrap(raw_obj);
            return;
        }
    }
    else if (!raw_arg.IsTypedArray())
    {
        throw Napi::TypeError::New(env, "PublicKeyArg must be a PublicKey instance or a 48/96 byte Uint8Array");
    }

    _bytes = Uint8ArrayArg{env, raw_arg, "PublicKeyArg"};
    _bytes.ValidateLength(_addon->_global_state->_public_key_compressed_length, _addon->_global_state->_public_key_uncompressed_length);
    if (_bytes.HasError())
    {
        _bytes.ThrowJsException();
    }
};

blst::P1 *PublicKeyArg::AsJacobian()
{
    if (_public_key)
    {
        // need to check this works to not duplicate if affine is already build
        if (!_public_key->_is_jacobian /* && !_public_key->_affine->is_inf() */)
        {
            _public_key->_jacobian.reset(new blst::P1{_public_key->_affine->to_jacobian()});
            _public_key->_is_jacobian = true;
        }
        return _public_key->_jacobian.get();
    }
    if (_jacobian.get()->is_inf())
    {
        _jacobian.reset(new blst::P1{_bytes.Data(), _bytes.ByteLength()});
    }
    return _jacobian.get();
}

blst::P1_Affine *PublicKeyArg::AsAffine()
{
    if (_public_key)
    {
        // need to check this works to not duplicate if affine is already build
        if (_public_key->_is_jacobian /* && !_public_key->_jacobian->is_inf() */)
        {
            _public_key->_affine.reset(new blst::P1_Affine{_public_key->_jacobian->to_affine()});
        }
        return _public_key->_affine.get();
    }
    if (_affine.get()->is_inf())
    {
        _affine.reset(new blst::P1_Affine{_bytes.Data(), _bytes.ByteLength()});
    }
    return _affine.get();
}

/**
 *
 *
 * PublicKeyArgArray
 *
 *
 */
PublicKeyArgArray::PublicKeyArgArray(const BlstTsAddon *module, const Napi::Env &env, const Napi::Value &raw_arg) : _keys{}
{
    if (!raw_arg.IsArray())
    {
        throw Napi::TypeError::New(env, "publicKeys argument must be of type PublicKeyArg[]");
    }
    Napi::Array arr = raw_arg.As<Napi::Array>();
    uint32_t length = arr.Length();
    _keys.reserve(length);
    for (uint32_t i = 0; i < length; i++)
    {
        _keys.push_back(PublicKeyArg{module, env, arr[i]});
    }
}