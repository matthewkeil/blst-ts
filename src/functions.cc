#include "functions.h"

namespace functions
{
    /**
     *
     *
     * AggregatePublicKeys
     *
     *
     */
    class AggregatePublicKeysWorker : public BlstAsyncWorker
    {
    public:
        blst::P1 _result;

        AggregatePublicKeysWorker(const Napi::CallbackInfo &info, size_t arg_position)
            : BlstAsyncWorker(info),
              _arg_position{arg_position},
              _public_keys{},
              _result{}
        {
        }

        void Setup() override
        {
            if (!_info[_arg_position].IsArray())
            {
                SetError("publicKeys argument must be of type PublicKeyArg[]");
            }
            Napi::Array arr = _info[_arg_position].As<Napi::Array>();
            for (size_t i = 0; i < arr.Length(); i++)
            {
                _public_keys[i] = std::move(PublicKeyArg{_module, _env, arr[i]});
            }
        };

        void Execute() override
        {
            for (size_t i = 0; i < _public_keys.size(); i++)
            {
                try
                {
                    _result.add(*_public_keys[i].AsJacobian());
                }
                catch (const blst::BLST_ERROR &err)
                {
                    std::ostringstream msg;
                    msg << "BLST_ERROR::" << _module->GetBlstErrorString(err) << ": Invalid key at index " << i;
                    SetError(msg.str());
                }
            }
        }

        Napi::Value GetReturnValue() override
        {
            Napi::Object wrapped = _module->_public_key_ctr.New({Napi::External<void *>::New(Env(), nullptr)});
            PublicKey *pk = PublicKey::Unwrap(wrapped);
            pk->_jacobian.reset(new blst::P1{_result});
            pk->_is_jacobian = true;
            return wrapped;
        };

    private:
        size_t _arg_position;
        std::vector<PublicKeyArg> _public_keys;
    };
    /**
     *
     *
     * AggregateSignatures
     *
     *
     */
    class AggregateSignaturesWorker : public BlstAsyncWorker
    {
    public:
        AggregateSignaturesWorker(
            const Napi::CallbackInfo &info)
            : BlstAsyncWorker(info),
              _result{},
              _signatures{} {}

    protected:
        void Setup() override
        {
            if (!_info[0].IsArray())
            {
                SetError("signatures argument must be of type SignatureArg[]");
            }
            Napi::Array arr = _info[0].As<Napi::Array>();
            for (size_t i = 0; i < arr.Length(); i++)
            {
                _signatures[i] = std::move(SignatureArg{_module, _env, arr[i]});
            }
        };

        void Execute() override
        {
            for (size_t i = 0; i < _signatures.size(); i++)
            {
                try
                {
                    _result.add(*_signatures[i].AsJacobian());
                }
                catch (const blst::BLST_ERROR &err)
                {
                    std::ostringstream msg;
                    msg << "BLST_ERROR::" << _module->GetBlstErrorString(err) << ": Invalid signature at index " << i;
                    SetError(msg.str());
                }
            }
        }

        Napi::Value GetReturnValue() override
        {
            Napi::Object wrapped = _module->_signature_ctr.New({Napi::External<void *>::New(Env(), nullptr)});
            Signature *sig = Signature::Unwrap(wrapped);
            sig->_jacobian.reset(new blst::P2{_result});
            sig->_is_jacobian = true;
            return wrapped;
        };

    private:
        blst::P2 _result;
        std::vector<SignatureArg> _signatures;
    };
    /**
     *
     *
     * AggregateVerify
     *
     *
     */
    class AggregateVerifyWorker : public BlstAsyncWorker
    {
    public:
        AggregateVerifyWorker(const Napi::CallbackInfo &info)
            : BlstAsyncWorker(info),
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _msgs{_env, _info[0]},
              _public_keys{_module, _env, _info[1]},
              _signature{_module, _env, _info[2]},
              _result{} {}

    protected:
        void Setup() override{};

        void Execute() override
        {
            for (size_t i = 0; i < _public_keys.size(); i++)
            {
                blst::BLST_ERROR err = _ctx->aggregate(
                    _public_keys[i].AsAffine(),
                    _signature.AsAffine(),
                    _msgs[i].Data(),
                    _msgs[i].ByteLength());
                if (err != blst::BLST_ERROR::BLST_SUCCESS)
                {
                    throw err;
                }
            }
            _ctx->commit();
            _result = _ctx->finalverify(&blst::PT(*_signature.AsAffine()));
        }

        Napi::Value GetReturnValue() override
        {
            return Napi::Boolean::New(_env, _result);
        };

    private:
        std::unique_ptr<blst::Pairing> _ctx;
        MessageArgArray _msgs;
        PublicKeyArgArray _public_keys;
        SignatureArg _signature;
        bool _result;
    };
    /**
     *
     *
     * Verify
     *
     *
     */
    class VerifyWorker : public BlstAsyncWorker
    {
    public:
        VerifyWorker(const Napi::CallbackInfo &info)
            : BlstAsyncWorker(info),
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _msg{_env, _info[0]},
              _public_key{_module, _env, _info[1]},
              _signature{_module, _env, _info[2]},
              _result{} {}

    protected:
        void Setup() override{};

        void Execute() override
        {
            blst::BLST_ERROR err = _ctx->aggregate(
                _public_key.AsAffine(),
                _signature.AsAffine(),
                _msg.Data(),
                _msg.ByteLength());
            if (err != blst::BLST_ERROR::BLST_SUCCESS)
            {
                throw err;
            }
            _ctx->commit();
            _result = _ctx->finalverify(&blst::PT(*_signature.AsAffine()));
        }

        Napi::Value GetReturnValue() override
        {
            return Napi::Boolean::New(_env, _result);
        };

    private:
        std::unique_ptr<blst::Pairing> _ctx;
        MessageArg _msg;
        PublicKeyArg _public_key;
        SignatureArg _signature;
        bool _result;
    };
    /**
     *
     *
     * FastAggregateVerify
     *
     *
     */
    class FastAggregateVerifyWorker : public BlstAsyncWorker
    {
    public:
        FastAggregateVerifyWorker(
            const Napi::CallbackInfo &info)
            : BlstAsyncWorker(info),
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _msg{_env, _info[0]},
              _pub_keys_agg_worker{_info, 1},
              _signature{_module, _env, _info[2]},
              _result{} {}

    protected:
        void Setup() override
        {
            _pub_keys_agg_worker.Setup();
        };

        void Execute() override
        {
            _pub_keys_agg_worker.Execute();
            blst::BLST_ERROR err = _ctx->aggregate(
                &_pub_keys_agg_worker._result.to_affine(),
                _signature.AsAffine(),
                _msg.Data(),
                _msg.ByteLength());
            if (err != blst::BLST_ERROR::BLST_SUCCESS)
            {
                throw err;
            }
            _ctx->commit();
            _result = _ctx->finalverify(&blst::PT(*_signature.AsAffine()));
        }

        Napi::Value GetReturnValue() override
        {
            return Napi::Boolean::New(_env, _result);
        };

    private:
        std::unique_ptr<blst::Pairing> _ctx;
        MessageArg _msg;
        AggregatePublicKeysWorker _pub_keys_agg_worker;
        SignatureArg _signature;
        bool _result;
    };
    /**
     *
     *
     * VerifyMultipleAggregateSignatures
     *
     *
     */
    class VerifyMultipleAggregateSignaturesWorker : public BlstAsyncWorker
    {
    public:
        VerifyMultipleAggregateSignaturesWorker(
            const Napi::CallbackInfo &info)
            : BlstAsyncWorker(info),
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _msgs{_env, _info[0]},
              _public_keys{_module, _env, _info[1]},
              _signatures{_module, _env, _info[2]},
              _result{} {}

    protected:
        void Setup() override{};

        void Execute() override
        {
            size_t length{_module->_global_state->_random_bytes_length};
            for (size_t i = 0; i < _msgs.size(); i++)
            {
                blst::byte rand[length];
                RAND_bytes(rand, length);
                blst::BLST_ERROR err = _ctx->mul_n_aggregate(_public_keys[i].AsAffine(),
                                                             _signatures[i].AsAffine(),
                                                             rand,
                                                             length,
                                                             _msgs[i].Data(),
                                                             _msgs[i].ByteLength());
                if (err != blst::BLST_ERROR::BLST_SUCCESS)
                {
                    throw err;
                }
            }
            _ctx->commit();
            _result = _ctx->finalverify();
        }

        Napi::Value GetReturnValue() override
        {
            return Napi::Boolean::New(_env, _result);
        };

    private:
        std::unique_ptr<blst::Pairing> _ctx;
        MessageArgArray _msgs;
        PublicKeyArgArray _public_keys;
        SignatureArgArray _signatures;
        bool _result;
    };

    Napi::Value AggregatePublicKeys(const Napi::CallbackInfo &info)
    {
        AggregatePublicKeysWorker *worker = new AggregatePublicKeysWorker{info, 0};
        return worker->Run();
    }
    Napi::Value AggregatePublicKeysSync(const Napi::CallbackInfo &info)
    {
        AggregatePublicKeysWorker worker{info, 0};
        return worker.Run();
    }
    Napi::Value AggregateSignatures(const Napi::CallbackInfo &info)
    {
        AggregateSignaturesWorker *worker = new AggregateSignaturesWorker{info};
        return worker->Run();
    }
    Napi::Value AggregateSignaturesSync(const Napi::CallbackInfo &info)
    {
        AggregateSignaturesWorker worker{info};
        return worker.Run();
    }
    Napi::Value Verify(const Napi::CallbackInfo &info)
    {
        VerifyWorker *worker = new VerifyWorker{info};
        return worker->Run();
    }
    Napi::Value VerifySync(const Napi::CallbackInfo &info)
    {
        VerifyWorker worker{info};
        return worker.Run();
    }
    Napi::Value AggregateVerify(const Napi::CallbackInfo &info)
    {
        AggregateVerifyWorker *worker = new AggregateVerifyWorker{info};
        return worker->Run();
    }
    Napi::Value AggregateVerifySync(const Napi::CallbackInfo &info)
    {
        AggregateVerifyWorker worker{info};
        return worker.Run();
    }
    Napi::Value FastAggregateVerify(const Napi::CallbackInfo &info)
    {
        FastAggregateVerifyWorker *worker = new FastAggregateVerifyWorker{info};
        return worker->Run();
    }
    Napi::Value FastAggregateVerifySync(const Napi::CallbackInfo &info)
    {
        FastAggregateVerifyWorker worker{info};
        return worker.Run();
    }
    Napi::Value VerifyMultipleAggregateSignatures(const Napi::CallbackInfo &info)
    {
        VerifyMultipleAggregateSignaturesWorker *worker = new VerifyMultipleAggregateSignaturesWorker{info};
        return worker->Run();
    }
    Napi::Value VerifyMultipleAggregateSignaturesSync(const Napi::CallbackInfo &info)
    {
        VerifyMultipleAggregateSignaturesWorker worker{info};
        return worker.Run();
    }

    void Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *addon)
    {
        exports.Set(Napi::String::New(env, "aggregatePublicKeys"), functions::AggregatePublicKeys);
        exports.Set(Napi::String::New(env, "aggregatePublicKeysSync"), functions::AggregatePublicKeysSync);
        exports.Set(Napi::String::New(env, "aggregateSignatures"), functions::AggregateSignatures);
        exports.Set(Napi::String::New(env, "aggregateSignaturesSync"), functions::AggregateSignaturesSync);
        exports.Set(Napi::String::New(env, "verify"), functions::Verify);
        exports.Set(Napi::String::New(env, "verifySync"), functions::VerifySync);
        exports.Set(Napi::String::New(env, "aggregateVerify"), functions::AggregateVerify);
        exports.Set(Napi::String::New(env, "aggregateVerifySync"), functions::AggregateVerifySync);
        exports.Set(Napi::String::New(env, "fastAggregateVerify"), functions::FastAggregateVerify);
        exports.Set(Napi::String::New(env, "fastAggregateVerifySync"), functions::FastAggregateVerifySync);
        exports.Set(Napi::String::New(env, "verifyMultipleAggregateSignatures"), functions::VerifyMultipleAggregateSignatures);
        exports.Set(Napi::String::New(env, "verifyMultipleAggregateSignaturesSync"), functions::VerifyMultipleAggregateSignaturesSync);
    };
}