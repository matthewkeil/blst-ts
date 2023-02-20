#include "signature.h"

void Signature::Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *module)
{
    auto proto = {
        StaticMethod("deserialize", &Signature::Deserialize, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        InstanceMethod("serialize", &Signature::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("sigValidate", &Signature::SigValidate, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("sigValidateSync", &Signature::SigValidateSync, static_cast<napi_property_attributes>(napi_enumerable)),
        /**
         * This should be switched to the resolution of this ticket.
         * https://github.com/nodejs/node-addon-api/issues/1260
         *
         * Until then query through JS to make sure the object passed through from JS
         * is the correct type to prevent seg fault
         */
        InstanceValue("__type", Napi::String::New(env, module->_global_state->_signature_type), static_cast<napi_property_attributes>(napi_default)),
    };
    Napi::Function ctr = DefineClass(env, "Signature", proto, module);
    module->_signature_ctr = Napi::Persistent(ctr);
    exports.Set(Napi::String::New(env, "Signature"), ctr);
}

/**
 *
 *
 * Signature Workers
 *
 *
 */
namespace
{
    class SigValidateWorker : public BlstAsyncWorker
    {
    public:
        SigValidateWorker(
            const Napi::CallbackInfo &info, bool is_jacobian, blst::P2 &jacobian, blst::P2_Affine &affine)
            : BlstAsyncWorker(info),
              _is_jacobian{true},
              _jacobian{jacobian},
              _affine{affine} {};

    protected:
        void Setup() override{};
        ;

        void Execute() override
        {
            if (_is_jacobian)
            {
                if (!_jacobian.in_group())
                {
                    SetError("blst::BLST_POINT_NOT_IN_GROUP");
                }
            }
            else if (!_affine.in_group())
            {
                SetError("blst::BLST_POINT_NOT_IN_GROUP");
            }
        };

        Napi::Value GetReturnValue() override
        {
            return _env.Undefined();
        };

    private:
        bool _is_jacobian;
        blst::P2 &_jacobian;
        blst::P2_Affine &_affine;
    };
}

/**
 *
 *
 * Signature Methods
 *
 *
 */
Napi::Value Signature::Deserialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    BlstTsAddon *module = env.GetInstanceData<BlstTsAddon>();
    Napi::Object wrapped = module->_signature_ctr.New({Napi::External<void>::New(env, nullptr)});
    Signature *sig = Signature::Unwrap(wrapped);
    sig->_is_jacobian = true;
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
            sig->_is_jacobian = false;
        }
    }

    Uint8ArrayArg sig_bytes{
        info,
        0,
        "sigBytes",
    };
    sig_bytes.ValidateLength(module->_global_state->_signature_compressed_length, module->_global_state->_signature_uncompressed_length);
    if (sig_bytes.HasError())
    {
        sig_bytes.ThrowJsException();
        return env.Undefined();
    }
    try
    {
        if (sig->_is_jacobian)
        {
            sig->_jacobian.reset(new blst::P2{sig_bytes.Data(), sig_bytes.ByteLength()});
        }
        else
        {
            sig->_affine.reset(new blst::P2_Affine{sig_bytes.Data(), sig_bytes.ByteLength()});
        }
    }
    catch (blst::BLST_ERROR err)
    {
        Napi::RangeError::New(env, module->GetBlstErrorString(err)).ThrowAsJavaScriptException();
        return env.Undefined();
    }
    return wrapped;
}

Signature::Signature(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<Signature>{info},
      _is_jacobian{false},
      _jacobian{nullptr},
      _affine{nullptr},
      _module{reinterpret_cast<BlstTsAddon *>(info.Data())}
{
    Napi::Env env = info.Env();
    if (!info[0].IsExternal())
    {
        Napi::Error::New(env, "Signature constructor is private").ThrowAsJavaScriptException();
        return;
    }
};

Napi::Value Signature::Serialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    bool compressed{true};
    if (!info[0].IsUndefined())
    {
        compressed = info[0].ToBoolean().Value();
    }
    size_t length = compressed ? _module->_global_state->_signature_compressed_length : _module->_global_state->_signature_uncompressed_length;
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

Napi::Value Signature::SigValidate(const Napi::CallbackInfo &info)
{
    SigValidateWorker *worker = new SigValidateWorker{info, _is_jacobian, *_jacobian, *_affine};
    return worker->Run();
}

Napi::Value Signature::SigValidateSync(const Napi::CallbackInfo &info)
{
    SigValidateWorker worker{info, _is_jacobian, *_jacobian, *_affine};
    return worker.RunSync();
}

/**
 *
 *
 * SignatureArg
 *
 *
 */
SignatureArg::SignatureArg(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg)
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

const blst::P2 *SignatureArg::AsJacobian()
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

const blst::P2_Affine *SignatureArg::AsAffine()
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

/**
 *
 *
 * SignatureArgArray
 *
 *
 */
SignatureArgArray::SignatureArgArray(const BlstTsAddon *module, const Napi::Env &env, const Napi::Value &raw_arg) : SignatureArgArray{}
{
    if (!raw_arg.IsArray())
    {
        throw Napi::TypeError::New(env, "publicKeys argument must be of type SignatureArg[]");
    }
    Napi::Array arr = raw_arg.As<Napi::Array>();
    for (size_t i = 0; i < arr.Length(); i++)
    {
        (*this)[i] = SignatureArg{module, env, arr[i]};
    }
}