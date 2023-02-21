#include "functions.h"

namespace
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
        AggregatePublicKeysWorker(const Napi::CallbackInfo &info, size_t arg_position)
            : BlstAsyncWorker(info),
              _result{},
              _public_keys{_module, info, arg_position} {}

        void Setup() override
        {
            if (_public_keys.HasError())
            {
                SetError(_public_keys.GetError());
            }
        };

        void Execute() override
        {
            for (size_t i = 0; i < _public_keys.Size(); i++)
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

        blst::P1 &GetAggregate() { return _result; };

    private:
        blst::P1 _result;
        PublicKeyArgArray _public_keys;
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
        AggregateSignaturesWorker(const Napi::CallbackInfo &info, size_t arg_position)
            : BlstAsyncWorker(info),
              _result{},
              _signatures{_module, info, arg_position} {}

    protected:
        void Setup() override
        {
            if (_signatures.HasError())
            {
                SetError(_signatures.GetError());
            }
        };

        void Execute() override
        {
            for (size_t i = 0; i < _signatures.Size(); i++)
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
        SignatureArgArray _signatures;
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
              _result{false},
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _msg{_env, _info[0], "msg"},
              _public_key{_module, _env, _info[1]},
              _signature{_module, _env, _info[2]} {}

    protected:
        void Setup() override
        {
            if (_msg.HasError())
            {
                SetError(_msg.GetError());
                return;
            }
            if (_public_key.HasError())
            {
                SetError(_public_key.GetError());
                return;
            }
            if (_signature.HasError())
            {
                SetError(_signature.GetError());
                return;
            }
        };

        void Execute() override
        {
            blst::BLST_ERROR err = _ctx->aggregate(
                _public_key.AsAffine(),
                _signature.AsAffine(),
                _msg.Data(),
                _msg.ByteLength());
            if (err != blst::BLST_ERROR::BLST_SUCCESS)
            {
                std::ostringstream msg;
                msg << "BLST_ERROR::" << _module->GetBlstErrorString(err) << ": Invalid verification";
                SetError(msg.str());
                return;
            }
            _ctx->commit();
            blst::PT pt{*_signature.AsAffine()};
            _result = _ctx->finalverify(&pt);
        }

        Napi::Value GetReturnValue() override
        {
            return Napi::Boolean::New(_env, _result);
        };

    private:
        bool _result;
        std::unique_ptr<blst::Pairing> _ctx;
        Uint8ArrayArg _msg;
        PublicKeyArg _public_key;
        SignatureArg _signature;
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
              _result{},
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _msgs{info, 0, "msgs", "msgs"},
              _public_keys{_module, _env, _info[1]},
              _signature{_module, _env, _info[2]} {}

    protected:
        void Setup() override{};

        void Execute() override
        {
            for (size_t i = 0; i < _public_keys.Size(); i++)
            {
                blst::BLST_ERROR err = _ctx->aggregate(
                    _public_keys[i].AsAffine(),
                    _signature.AsAffine(),
                    _msgs[i].Data(),
                    _msgs[i].ByteLength());
                if (err != blst::BLST_ERROR::BLST_SUCCESS)
                {
                    std::ostringstream msg;
                    msg << "BLST_ERROR::" << _module->GetBlstErrorString(err) << ": Invalid verification at index " << i;
                    SetError(msg.str());
                    return;
                }
            }
            _ctx->commit();
            blst::PT pt(*_signature.AsAffine());
            _result = _ctx->finalverify(&pt);
        }

        Napi::Value GetReturnValue() override
        {
            return Napi::Boolean::New(_env, _result);
        };

    private:
        bool _result;
        std::unique_ptr<blst::Pairing> _ctx;
        Uint8ArrayArgArray _msgs;
        PublicKeyArgArray _public_keys;
        SignatureArg _signature;
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
              _result{},
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _msg{_env, _info[0], "msg"},
              _pub_keys_agg_worker{_info, 1},
              _signature{_module, _env, _info[2]} {}

    protected:
        void Setup() override
        {
            if (_msg.HasError())
            {
                SetError(_msg.GetError());
                return;
            }
            _pub_keys_agg_worker.Setup();
            if (_pub_keys_agg_worker.HasError())
            {
                SetError(_pub_keys_agg_worker.GetError());
                return;
            }
            if (_signature.HasError())
            {
                SetError(_signature.GetError());
                return;
            }
        };

        void Execute() override
        {
            _pub_keys_agg_worker.Execute();
            auto a = _pub_keys_agg_worker.GetAggregate().to_affine();
            blst::BLST_ERROR err = _ctx->aggregate(
                &a,
                _signature.AsAffine(),
                _msg.Data(),
                _msg.ByteLength());
            if (err != blst::BLST_ERROR::BLST_SUCCESS)
            {
                throw err;
            }
            _ctx->commit();
            blst::PT pt(*_signature.AsAffine());
            _result = _ctx->finalverify(&pt);
        }

        Napi::Value GetReturnValue() override
        {
            return Napi::Boolean::New(_env, _result);
        };

    private:
        bool _result;
        std::unique_ptr<blst::Pairing> _ctx;
        Uint8ArrayArg _msg;
        AggregatePublicKeysWorker _pub_keys_agg_worker;
        SignatureArg _signature;
    };
    /**
     *
     *
     * VerifyMultipleAggregateSignatures
     *
     *
     */
    class SignatureSet
    {
    public:
        Uint8ArrayArg _msg;
        PublicKeyArg _publicKey;
        SignatureArg _signature;

        SignatureSet(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg)
            : _msg{_env},
              _publicKey{addon, _env},
              _signature{addon, _env},
              _addon{addon},
              _env{env},
              _error{}
        {
            if (!raw_arg.IsObject())
            {
                SetError("SignatureSet must be an object");
                return;
            }
            Napi::Object set = raw_arg.As<Napi::Object>();
            if (!set.Has("msg"))
            {
                SetError("SignatureSet must have a 'msg' property");
                return;
            }
            _msg = Uint8ArrayArg{_env, set.Get("msg"), "msg"};
            if (!set.Has("publicKey"))
            {
                SetError("SignatureSet must have a 'publicKey' property");
                return;
            }
            _publicKey = PublicKeyArg{_addon, _env, set.Get("publicKey")};
            if (!set.Has("signature"))
            {
                SetError("SignatureSet must have a 'signature' property");
                return;
            }
            _signature = SignatureArg{_addon, _env, set.Get("signature")};
        };

        void ThrowJsException() { Napi::Error::New(_env, _error).ThrowAsJavaScriptException(); };
        bool HasError() { return _error.size() > 0; };
        std::string GetError() { return _error; };

    private:
        const BlstTsAddon *_addon;
        Napi::Env _env;
        std::string _error;

        void SetError(const std::string &err) { _error = err; };
    };
    class SignatureSetArray
    {
    public:
        std::vector<SignatureSet> _sets;

        SignatureSetArray(const BlstTsAddon *addon, const Napi::Env &env, const Napi::Value &raw_arg)
            : _sets{},
              _env{env},
              _error{}
        {
            if (!raw_arg.IsArray())
            {
                SetError("signatureSets must be of type SignatureSet[]");
                return;
            }
            Napi::Array arr = raw_arg.As<Napi::Array>();
            uint32_t length = arr.Length();
            _sets.reserve(length);
            for (uint32_t i = 0; i < length; i++)
            {
                _sets.push_back(SignatureSet{addon, env, arr[i]});
                if (_sets[i].HasError())
                {
                    SetError(_sets[i].GetError());
                    return;
                }
            }
        };
        SignatureSetArray(const SignatureSetArray &source) = delete;
        SignatureSetArray(SignatureSetArray &&source) = default;

        SignatureSetArray &operator=(const SignatureSetArray &source) = delete;
        SignatureSetArray &operator=(SignatureSetArray &&source) = default;
        SignatureSet &operator[](size_t index)
        {
            return _sets[index];
        }

        size_t Size() { return _sets.size(); }
        void Reserve(size_t size) { return _sets.reserve(size); }
        void ThrowJsException() { Napi::Error::New(_env, _error).ThrowAsJavaScriptException(); };
        bool HasError() { return _error.size() > 0; };
        std::string GetError() { return _error; };

    private:
        Napi::Env _env;
        std::string _error;

        void SetError(const std::string &err) { _error = err; };
    };

    class VerifyMultipleAggregateSignaturesWorker : public BlstAsyncWorker
    {
    public:
        VerifyMultipleAggregateSignaturesWorker(
            const Napi::CallbackInfo &info)
            : BlstAsyncWorker(info),
              _result{true},
              _ctx{std::make_unique<blst::Pairing>(true, _module->_global_state->_dst)},
              _sets{_module, _env, _info[0]} {}

    protected:
        void Setup() override
        {
            if (_sets.HasError())
            {
                SetError(_sets.GetError());
                return;
            }
        };

        void Execute() override
        {
            size_t length{_module->_global_state->_random_bytes_length};
            for (size_t i = 0; i < _sets.Size(); i++)
            {
                blst::byte rand[length];
                _module->GetRandomBytes(rand, length);
                blst::BLST_ERROR err = _ctx->mul_n_aggregate(_sets[i]._publicKey.AsAffine(),
                                                             _sets[i]._signature.AsAffine(),
                                                             rand,
                                                             length,
                                                             _sets[i]._msg.Data(),
                                                             _sets[i]._msg.ByteLength());
                if (err != blst::BLST_ERROR::BLST_SUCCESS)
                {
                    std::ostringstream msg;
                    msg << "BLST_ERROR::" << _module->GetBlstErrorString(err) << ": Invalid aggregation at index " << i;
                    SetError(msg.str());
                    return;
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
        bool _result;
        std::unique_ptr<blst::Pairing> _ctx;
        SignatureSetArray _sets;
    };
} // namespace (anonymous)

namespace functions
{

    Napi::Value AggregatePublicKeys(const Napi::CallbackInfo &info)
    {
        AggregatePublicKeysWorker *worker = new AggregatePublicKeysWorker{info, 0};
        return worker->Run();
    }
    Napi::Value AggregatePublicKeysSync(const Napi::CallbackInfo &info)
    {
        AggregatePublicKeysWorker worker{info, 0};
        return worker.RunSync();
    }
    Napi::Value AggregateSignatures(const Napi::CallbackInfo &info)
    {
        AggregateSignaturesWorker *worker = new AggregateSignaturesWorker{info, 0};
        return worker->Run();
    }
    Napi::Value AggregateSignaturesSync(const Napi::CallbackInfo &info)
    {
        AggregateSignaturesWorker worker{info, 0};
        return worker.RunSync();
    }
    Napi::Value Verify(const Napi::CallbackInfo &info)
    {
        VerifyWorker *worker = new VerifyWorker{info};
        return worker->Run();
    }
    Napi::Value VerifySync(const Napi::CallbackInfo &info)
    {
        VerifyWorker worker{info};
        return worker.RunSync();
    }
    Napi::Value AggregateVerify(const Napi::CallbackInfo &info)
    {
        AggregateVerifyWorker *worker = new AggregateVerifyWorker{info};
        return worker->Run();
    }
    Napi::Value AggregateVerifySync(const Napi::CallbackInfo &info)
    {
        AggregateVerifyWorker worker{info};
        return worker.RunSync();
    }
    Napi::Value FastAggregateVerify(const Napi::CallbackInfo &info)
    {
        FastAggregateVerifyWorker *worker = new FastAggregateVerifyWorker{info};
        return worker->Run();
    }
    Napi::Value FastAggregateVerifySync(const Napi::CallbackInfo &info)
    {
        FastAggregateVerifyWorker worker{info};
        return worker.RunSync();
    }
    Napi::Value VerifyMultipleAggregateSignatures(const Napi::CallbackInfo &info)
    {
        VerifyMultipleAggregateSignaturesWorker *worker = new VerifyMultipleAggregateSignaturesWorker{info};
        return worker->Run();
    }
    Napi::Value VerifyMultipleAggregateSignaturesSync(const Napi::CallbackInfo &info)
    {
        VerifyMultipleAggregateSignaturesWorker worker{info};
        return worker.RunSync();
    }

    void Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *addon)
    {
        exports.Set(Napi::String::New(env, "aggregatePublicKeys"), Napi::Function::New(env, AggregatePublicKeys));
        exports.Set(Napi::String::New(env, "aggregatePublicKeysSync"), Napi::Function::New(env, AggregatePublicKeysSync));
        exports.Set(Napi::String::New(env, "aggregateSignatures"), Napi::Function::New(env, AggregateSignatures));
        exports.Set(Napi::String::New(env, "aggregateSignaturesSync"), Napi::Function::New(env, AggregateSignaturesSync));
        exports.Set(Napi::String::New(env, "verify"), Napi::Function::New(env, Verify));
        exports.Set(Napi::String::New(env, "verifySync"), Napi::Function::New(env, VerifySync));
        exports.Set(Napi::String::New(env, "aggregateVerify"), Napi::Function::New(env, AggregateVerify));
        exports.Set(Napi::String::New(env, "aggregateVerifySync"), Napi::Function::New(env, AggregateVerifySync));
        exports.Set(Napi::String::New(env, "fastAggregateVerify"), Napi::Function::New(env, FastAggregateVerify));
        exports.Set(Napi::String::New(env, "fastAggregateVerifySync"), Napi::Function::New(env, FastAggregateVerifySync));
        exports.Set(Napi::String::New(env, "verifyMultipleAggregateSignatures"), Napi::Function::New(env, VerifyMultipleAggregateSignatures));
        exports.Set(Napi::String::New(env, "verifyMultipleAggregateSignaturesSync"), Napi::Function::New(env, VerifyMultipleAggregateSignaturesSync));
    };

} // functions namespace