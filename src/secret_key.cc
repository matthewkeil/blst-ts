#include "secret_key.h"

void SecretKey::Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *module)
{
    auto proto = {
        StaticMethod("fromKeygen", &SecretKey::FromKeygen, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        StaticMethod("fromKeygenSync", &SecretKey::FromKeygenSync, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        StaticMethod("deserialize", &SecretKey::Deserialize, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        InstanceMethod("serialize", &SecretKey::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("toPublicKey", &SecretKey::ToPublicKey, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("sign", &SecretKey::Sign, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("signSync", &SecretKey::SignSync, static_cast<napi_property_attributes>(napi_enumerable)),
        /**
         * This should be switched to the resolution of this ticket.
         * https://github.com/nodejs/node-addon-api/issues/1260
         *
         * Until then query through JS to make sure the object passed through from JS
         * is the correct type to prevent seg fault
         */
        InstanceValue("__type", Napi::String::New(env, module->_global_state->_secret_key_type), static_cast<napi_property_attributes>(napi_default)),
    };
    Napi::Function ctr = DefineClass(env, "SecretKey", proto, module);
    module->_secret_key_ctr = Napi::Persistent(ctr);
    exports.Set(Napi::String::New(env, "SecretKey"), ctr);
}

/**
 *
 *
 * SecretKey Workers
 *
 *
 */
namespace
{
    class FromKeygenWorker : public BlstAsyncWorker
    {
    public:
        FromKeygenWorker(const Napi::CallbackInfo &info)
            : BlstAsyncWorker(info),
              _key{},
              _data{nullptr},
              _entropy_array_ref{} {};

        void Setup() override
        {
            if (_info[0].IsUndefined()) // no entropy passed
            {
                _data = nullptr;
                return;
            }
            ARG_UINT8_OF_LENGTH_RETURN_VOID(_info, _env, 0, entropy, _module->_global_state->_secret_key_length, "ikm");
            _entropy_array_ref = Napi::Persistent<Napi::TypedArrayOf<u_int8_t>>(entropy);
            _data = entropy.Data();
        };

        void Execute() override
        {
            size_t sk_length = _module->_global_state->_secret_key_length;
            if (_data == nullptr) // no entropy passed
            {
                blst::byte ikm[sk_length];
                RAND_bytes(ikm, sk_length);
                _key.keygen(ikm, sk_length);
            }
            else
            {
                _key.keygen(_data, sk_length);
            }
            _entropy_array_ref.Reset();
        };

        Napi::Value GetReturnValue() override
        {
            Napi::Object wrapped = _module->_secret_key_ctr.New({Napi::External<void *>::New(Env(), nullptr)});
            SecretKey *sk = SecretKey::Unwrap(wrapped);
            sk->_key.reset(new blst::SecretKey{_key});
            return wrapped;
        };

    private:
        blst::SecretKey _key;
        uint8_t *_data;
        Napi::Reference<Napi::TypedArrayOf<u_int8_t>> _entropy_array_ref;
    };

    class SignWorker : public BlstAsyncWorker
    {
    public:
        SignWorker(const Napi::CallbackInfo &info, const blst::SecretKey &key)
            : BlstAsyncWorker{info}, _key{key}, _point{}, _msg_array_ref{}, _msg{}, _msg_length{} {};
        void Setup() override
        {
            ARG_TO_UINT8_RETURN_VOID(_info, _env, 0, msg, "msg to sign");
            _msg_array_ref = Napi::Reference<Napi::TypedArrayOf<u_int8_t>>::New(msg, 1);
            _msg = msg.Data();
            _msg_length = msg.ByteLength();
        };
        void Execute() override
        {
            _point.hash_to(_msg, _msg_length, _module->_global_state->dst_);
            _point.sign_with(_key);
        };
        Napi::Value GetReturnValue() override
        {
            Napi::Object wrapped = _module->_signature_ctr.New({Napi::External<void *>::New(Env(), nullptr)});
            Signature *sig = Signature::Unwrap(wrapped);
            sig->_jacobian.reset(new blst::P2{_point});
            return wrapped;
        };

    private:
        const blst::SecretKey &_key;
        blst::P2 _point;
        Napi::Reference<Napi::TypedArrayOf<u_int8_t>> _msg_array_ref;
        uint8_t *_msg;
        size_t _msg_length;
    };
} // namespace (anonymous)

/**
 *
 *
 * SecretKey
 *
 *
 */
Napi::Value SecretKey::FromKeygen(const Napi::CallbackInfo &info)
{
    FromKeygenWorker *worker = new FromKeygenWorker{info};
    return worker->Run();
}

Napi::Value SecretKey::FromKeygenSync(const Napi::CallbackInfo &info)
{
    FromKeygenWorker worker{info};
    Napi::Value key = worker.RunSync();
    return key;
}

Napi::Value SecretKey::Deserialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    BlstTsAddon *module = env.GetInstanceData<BlstTsAddon>();
    ARG_UINT8_OF_LENGTH_RETURN_UNDEFINED(info, env, 0, skBytes, module->_global_state->_secret_key_length, "skBytes");
    Napi::Object wrapped = module->_secret_key_ctr.New({Napi::External<void>::New(env, nullptr)});
    SecretKey *sk = SecretKey::Unwrap(wrapped);
    sk->_key->from_bendian(skBytes.Data());
    return wrapped;
}

SecretKey::SecretKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<SecretKey>{info},
      _key{new blst::SecretKey},
      _module{reinterpret_cast<BlstTsAddon *>(info.Data())}
{
    Napi::Env env = info.Env();
    if (!info[0].IsExternal())
    {
        Napi::Error::New(env, "SecretKey constructor is private").ThrowAsJavaScriptException();
        return;
    }
};

Napi::Value SecretKey::Serialize(const Napi::CallbackInfo &info)
{
    Napi::Env env = info.Env();
    Napi::Buffer<uint8_t> serialized = Napi::Buffer<uint8_t>::New(env, _module->_global_state->_secret_key_length);
    _key->to_bendian(serialized.Data());
    return serialized;
}

Napi::Value SecretKey::ToPublicKey(const Napi::CallbackInfo &info)
{
    Napi::Object wrapped = _module->_public_key_ctr.New({Napi::External<void *>::New(Env(), nullptr)});
    PublicKey *pk = PublicKey::Unwrap(wrapped);
    pk->_jacobian.reset(new blst::P1{*_key});
    pk->_is_jacobian = true;
    return wrapped;
}

Napi::Value SecretKey::Sign(const Napi::CallbackInfo &info)
{
    SignWorker worker{info, *_key};
    return worker.Run();
}

Napi::Value SecretKey::SignSync(const Napi::CallbackInfo &info)
{
    SignWorker worker{info, *_key};
    return worker.RunSync();
}
