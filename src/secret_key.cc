#include <sstream>
#include <openssl/rand.h>
#include "napi.h"
#include "blst.hpp"
#include "addon.h"
#include "macros.h"
#include "utils.h"
#include "secret_key.h"
// #include "public_key.h"
// #include "signature.h"

void SecretKey::Init(const Napi::Env &env, Napi::Object &exports, Napi::Function &ctr, BlstTsAddon *module)
{
    auto proto = {
        StaticMethod("fromKeygen", &SecretKey::FromKeygen, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        StaticMethod("fromKeygenSync", &SecretKey::FromKeygenSync, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        StaticMethod("deserialize", &SecretKey::Deserialize, static_cast<napi_property_attributes>(napi_static | napi_enumerable)),
        InstanceMethod("serialize", &SecretKey::Serialize, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("toPublicKey", &SecretKey::ToPublicKey, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("toPublicKeySync", &SecretKey::ToPublicKeySync, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("sign", &SecretKey::Sign, static_cast<napi_property_attributes>(napi_enumerable)),
        InstanceMethod("signSync", &SecretKey::SignSync, static_cast<napi_property_attributes>(napi_enumerable)),
        /**
         * This should be switched to the resolution of this ticket.
         * https://github.com/nodejs/node-addon-api/issues/1260
         *
         * Until then query through JS to make sure the object passed through from JS
         * is the correct type to prevent seg fault
         */
        InstanceValue("__type", Napi::String::New(env, module->global_state_->secret_key_type_), static_cast<napi_property_attributes>(napi_default)),
    };
    ctr = DefineClass(env, "SecretKey", proto, module);
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
            : BlstAsyncWorker(info), _key{}, _data{nullptr}, _entropy_array_ref{} {};
        void Setup() override
        {
            if (_info[0].IsUndefined()) // no entropy passed
            {
                _data = nullptr;
                return;
            }
            ARG_UINT8_OF_LENGTH_RETURN_VOID(_info, _env, 0, entropy, _module->global_state_->secret_key_length_, "IKM for new SecretKey(ikm)");
            _entropy_array_ref = Napi::Reference<Napi::TypedArrayOf<u_int8_t>>::New(entropy, 1);
            _data = entropy.Data();
        };
        void Execute() override
        {
            uint8_t sk_length = _module->global_state_->secret_key_length_;
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
            Napi::Object wrapped = _module->secret_key_ctr_.New({Napi::External<void>::New(Env(), nullptr)});
            SecretKey *sk = SecretKey::Unwrap(wrapped);
            sk->_key.reset(new blst::SecretKey{_key});
            return wrapped;
        };

    private:
        blst::SecretKey _key;
        uint8_t *_data;
        Napi::Reference<Napi::TypedArrayOf<u_int8_t>> _entropy_array_ref;
    };

    class ToPublicKeyWorker : public BlstAsyncWorker
    {
    public:
        ToPublicKeyWorker(const Napi::CallbackInfo &info, const blst::SecretKey &key)
            : BlstAsyncWorker{info}, _info{info}, _key{key}, _point{} {};
        void Setup() override{/* no-op */};
        void Execute() override { _point = blst::P1{_key}; };
        Napi::Value GetReturnValue() override
        {
            return _info.Env().Undefined();
            // Napi::Object wrapped = _module->secret_key_ctr_.New({Napi::External<void>::New(Env(), nullptr)});
            // PublicKey *pk = PublicKey::Unwrap(wrapped);
            // // TODO: check
            // pk->_point->add(_point);
            // // TODO: was like line below but think we can just add to the initialized point here,
            // //       check during PR and change other instances if not correct
            // // pk->_point.reset(new blst::P1{_point});
            // return wrapped;
        };

    private:
        const Napi::CallbackInfo &_info;
        const blst::SecretKey &_key;
        blst::P1 _point;
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
            _point.hash_to(_msg, _msg_length, _module->global_state_->dst_);
            _point.sign_with(_key);
        };
        Napi::Value GetReturnValue() override
        {
            return _info.Env().Undefined();
            // Napi::Object wrapped = _module->signature_ctr_.New({Napi::External<void>::New(Env(), nullptr)});
            // Signature *sig = Signature::Unwrap(wrapped);
            // // TODO: see note in ToPublicKeyWorker::GetReturnValue()
            // sig->_point->add(_point);
            // return wrapped;
        };

    private:
        const blst::SecretKey &_key;
        blst::P2 _point;
        Napi::Reference<Napi::TypedArrayOf<u_int8_t>> _msg_array_ref;
        uint8_t *_msg;
        size_t _msg_length;
    };
} // end anonymous namespace

/**
 *
 *
 * SecretKey
 *
 *
 */
Napi::Value SecretKey::FromKeygen(const Napi::CallbackInfo &info)
{
    FromKeygenWorker worker{info};
    return worker.Run();
}

Napi::Value SecretKey::FromKeygenSync(const Napi::CallbackInfo &info)
{
    FromKeygenWorker worker{info};
    return worker.RunSync();
}

Napi::Value SecretKey::Deserialize(const Napi::CallbackInfo &info)
{
    BlstTsAddon *_module{reinterpret_cast<BlstTsAddon *>(info.Data())};
    NAPI_ENV
    ARG_UINT8_OF_LENGTH_RETURN_UNDEFINED(info, env, 0, skBytes, _module->global_state_->secret_key_length_, "skBytes");
    Napi::Object wrapped = _module->secret_key_ctr_.New({Napi::External<void>::New(env, nullptr)});
    SecretKey *sk = SecretKey::Unwrap(wrapped);
    sk->_key->from_bendian(skBytes.Data());
    return wrapped;
}

SecretKey::SecretKey(const Napi::CallbackInfo &info)
    : Napi::ObjectWrap<SecretKey>{info},
      _key{new blst::SecretKey},
      _module{reinterpret_cast<BlstTsAddon *>(info.Data())}
{
    NAPI_ENV
    if (!info[0].IsExternal())
    {
        Napi::Error::New(env, "SecretKey constructor is private").ThrowAsJavaScriptException();
        return;
    }
};

Napi::Value SecretKey::Serialize(const Napi::CallbackInfo &info)
{
    NAPI_ENV
    Napi::Buffer<uint8_t> serialized = Napi::Buffer<uint8_t>::New(env, _module->global_state_->secret_key_length_);
    _key->to_bendian(serialized.Data());
    return serialized;
}

Napi::Value SecretKey::ToPublicKey(const Napi::CallbackInfo &info)
{
    ToPublicKeyWorker worker{info, *_key};
    return worker.Run();
}

Napi::Value SecretKey::ToPublicKeySync(const Napi::CallbackInfo &info)
{
    ToPublicKeyWorker worker{info, *_key};
    return worker.RunSync();
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

// class FromKeygenWorker : public BlstAsyncWorker
// {
// public:
//     FromKeygenWorker(const Napi::CallbackInfo &info) : BlstAsyncWorker(info) {};
//     void Setup() override;
//     void Execute() override;
//     Napi::Value GetReturnValue() override;

// private:
//     blst::SecretKey _key;
//     uint8_t *_data;
//     Napi::Reference<Napi::TypedArrayOf<u_int8_t>> _entropy_array_ref;
// };

// class ToPublicKeyWorker : public BlstAsyncWorker
// {
// public:
//     ToPublicKeyWorker(const Napi::CallbackInfo &info, const blst::SecretKey &key)
//         : BlstAsyncWorker{info}, _info{info}, _key{key}, _point{} {};
//     void Setup() override{/* no-op */};
//     void Execute() override { _point = blst::P1{_key}; };
//     Napi::Value GetReturnValue() override;

// private:
//     const Napi::CallbackInfo &_info;
//     const blst::SecretKey &_key;
//     blst::P1 _point;
// };

// class SignWorker : public BlstAsyncWorker
// {
// public:
//     SignWorker(const Napi::CallbackInfo &info) : BlstAsyncWorker(info), _info{info} {};
//     void Setup() override;
//     void Execute() override;
//     Napi::Value GetReturnValue() override;

// private:
//     const Napi::CallbackInfo &_info;
//     const blst::SecretKey &_key;
//     blst::P2 _point;
//     Napi::Reference<Napi::TypedArrayOf<u_int8_t>> _msg_array_ref;
//     uint8_t *_msg;
//     size_t _msg_length
// };
//
// void FromKeygenWorker::Setup()
// {
//     if (_info[0].IsUndefined()) // no entropy passed
//     {
//         _data = nullptr;
//         return;
//     }
//     ARG_NUM_TO_UINT8_ARRAY_OF_LENGTH(_info, _env, 0, entropy, _module->global_state_->secret_key_length_, "IKM for new SecretKey(ikm)");
//     _entropy_array_ref = Napi::Reference<Napi::TypedArrayOf<u_int8_t>>::New(entropy, 1);
//     _data = entropy.Data();
// }

// void FromKeygenWorker::Execute()
// {
//     uint8_t sk_length = _module->global_state_->secret_key_length_;
//     if (_data == nullptr) // no entropy passed
//     {
//         blst::byte ikm[sk_length];
//         RAND_bytes(ikm, sk_length);
//         _key.keygen(ikm, sk_length);
//     }
//     else
//     {
//         _key.keygen(_data, sk_length);
//     }
//     _entropy_array_ref.Reset();
// }

// Napi::Value FromKeygenWorker::GetReturnValue()
// {
//     Napi::Object wrapped = _module->secret_key_ctr_.New({Napi::External<void>::New(Env(), nullptr)});
//     SecretKey *sk = SecretKey::Unwrap(wrapped);
//     sk->_key.reset(new blst::SecretKey{_key});
//     return wrapped;
// }

// Napi::Value ToPublicKeyWorker::GetReturnValue()
// {
//     Napi::Object wrapped = _module->secret_key_ctr_.New({Napi::External<void>::New(Env(), nullptr)});
//     PublicKey *pk = PublicKey::Unwrap(wrapped);
//     // TODO: check
//     pk->_point->add(_point);
//     // TODO: was like line below but think we can just add to the initialized point here,
//     //       check during PR and change other instances if not correct
//     // pk->_point.reset(new blst::P1{_point});
//     return wrapped;
// }

// void SignWorker::Setup()
// {
//     ARG_NUM_TO_UINT8_ARRAY(_info, _env, 0, msg, "msg to sign");
//     _msg_array_ref = Napi::Reference<Napi::TypedArrayOf<u_int8_t>>::New(msg, 1);
//     _msg = msg.Data();
//     _msg_length = msg.ByteLength();
// }

// void SignWorker::Execute()
// {
//     _point.hash_to(_msg, _msg_length, _module->global_state_->dst_);
//     _point.sign_with(_key);
// }

// Napi::Value SignWorker::GetReturnValue()
// {
//     Napi::Object wrapped = _module->signature_ctr_.New({Napi::External<void>::New(Env(), nullptr)});
//     Signature *sig = Signature::Unwrap(wrapped);
//     // TODO: see note in ToPublicKeyWorker::GetReturnValue()
//     sig->_point->add(_point);
//     return wrapped;
// }