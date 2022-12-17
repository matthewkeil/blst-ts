#include <sstream>
#include <future>
#include <map>
#include <optional>
#include <string>
#include "napi.h"
#include "../blst_ts_utils.hpp"
#include "../public_key.hpp"
#include "../signature.hpp"
#include "../utils/thread_pool.h"

namespace agg_verify
{
    /**
     *
     *
     *
     *
     *
     *
     *
     */
    void GetMessage(const Napi::Env &env,
                    size_t index,
                    const Napi::Array &msgs_arr,
                    std::vector<ByteArray> &msgs)
    {
        std::string msg_name{"Message"};
        Napi::Value value = msgs_arr[index];
        ByteArray bytes = TryAsStringOrUint8Array(env, value, msg_name, true);
        if (bytes.ByteLength() > 0)
        {
            msgs[index] = std::move(bytes);
            return;
        }
        throw Napi::TypeError::New(env, msg_name + " must be a string or Uint8Array");
    }

    void GetPublicKey(
        const Napi::Env &env,
        size_t index,
        const Napi::Array &pub_keys_arr,
        std::vector<ByteArray> &public_keys,
        std::map<size_t, PublicKey *> &public_key_map)
    {
        std::string key_name{"PublicKey"};
        Napi::Value value = pub_keys_arr[index];
        ByteArray bytes = TryAsStringOrUint8Array(env, value, key_name, false);
        if (bytes.ByteLength() > 0)
        {
            public_keys[index] = std::move(bytes);
            return;
        }
        Napi::Object pk_obj = value.As<Napi::Object>();
        if (pk_obj.Has("__type") && (pk_obj.Get("__type").As<Napi::String>().Utf8Value().compare(PublicKey::kType_) == 0))
        {
            PublicKey *pk = PublicKey::Unwrap(pk_obj);
            if (pk != nullptr)
            {
                public_key_map[index] = pk;
                return;
            }
        }
        throw Napi::TypeError::New(env, key_name + " must be a string, Uint8Array or PublicKey");
    }

    blst::P2_Affine GetSignature(const Napi::Env &env, Napi::Value &value)
    {
        ByteArray bytes = TryAsStringOrUint8Array(env, value, "Signature", false);
        if (bytes.ByteLength() > 0)
        {
            return blst::P2_Affine{bytes.Data(), bytes.ByteLength()};
        }
        Napi::Object sig_obj = value.As<Napi::Object>();
        if (sig_obj.Has("__type") && (sig_obj.Get("__type").As<Napi::String>().Utf8Value().compare(Signature::kType_) == 0))
        {
            Signature *sig = Signature::Unwrap(sig_obj);
            if (sig != nullptr)
            {
                return sig->AsAffine();
            }
        }
        throw Napi::TypeError::New(env, "signature must be a string, Uint8Array or Signature");
    }

    void ParseMessagesAndKeys(const Napi::Env &env,
                              size_t set_length,
                              const Napi::Array &msgs_arr,
                              const Napi::Array &public_keys_arr,
                              std::vector<ByteArray> &msgs,
                              std::vector<ByteArray> &public_keys,
                              std::map<size_t, PublicKey *> &public_key_map)
    {
        for (size_t i = 0; i < set_length; i++)
        {
            GetMessage(env, i, msgs_arr, msgs);
            GetPublicKey(env, i, public_keys_arr, public_keys, public_key_map);
        }
    }

    bool AggregateSync(
        size_t set_length,
        std::unique_ptr<blst::Pairing> &ctx,
        blst::P2_Affine &signature,
        std::vector<ByteArray> &msgs,
        std::vector<ByteArray> &public_keys,
        std::map<size_t, PublicKey *> &public_key_map)
    {
        for (size_t i = 0; i < set_length; i++)
        {
            blst::P1_Affine pk;
            auto pk_iter = public_key_map.find(i);
            if (pk_iter != public_key_map.end())
            {
                pk = pk_iter->second->AsAffine();
            }
            else
            {
                pk = blst::P1_Affine{public_keys[i].Data(), public_keys[i].ByteLength()};
            }

            blst::BLST_ERROR err = ctx->aggregate(&pk,
                                                  &signature,
                                                  msgs[i].Data(),
                                                  msgs[i].ByteLength());
            if (err != blst::BLST_ERROR::BLST_SUCCESS)
            {
                throw err;
            }
        }
        ctx->commit();
        blst::PT gt_sig{signature};
        return ctx->finalverify(&gt_sig);
    }

    void AggregateThread(
        size_t index,
        std::unique_ptr<blst::Pairing> &ctx,
        std::mutex &ctx_mutex,
        blst::P2_Affine &sig,
        std::vector<ByteArray> &msgs,
        std::vector<ByteArray> &public_keys,
        std::map<size_t, PublicKey *> &public_key_map)
    {
        blst::P1_Affine pk;
        auto pk_iter = public_key_map.find(index);
        if (pk_iter != public_key_map.end())
        {
            pk = pk_iter->second->AsAffine();
        }
        else
        {
            pk = blst::P1_Affine{public_keys[index].Data(), public_keys[index].ByteLength()};
        }

        blst::Pairing *thread_ctx = new blst::Pairing(true, DST);
        blst::BLST_ERROR err = thread_ctx->aggregate(&pk,
                                                     &sig,
                                                     msgs[index].Data(),
                                                     msgs[index].ByteLength());
        if (err != blst::BLST_ERROR::BLST_SUCCESS)
        {
            throw err;
        }
        thread_ctx->commit();
        std::lock_guard<std::mutex> guard{ctx_mutex};
        ctx->merge(thread_ctx);
    }

    bool RunAggregation(
        size_t set_length,
        std::unique_ptr<blst::Pairing> &ctx,
        std::mutex &ctx_mutex,
        blst::P2_Affine &signature,
        std::vector<ByteArray> &msgs,
        std::vector<ByteArray> &public_keys,
        std::map<size_t, PublicKey *> &public_key_map,
        size_t thread_count)
    {
        ThreadPool pool{thread_count};
        std::future<void> results[set_length];
        for (size_t i = 0; i < set_length; i++)
        {
            results[i] = pool.submit(std::bind(&AggregateThread,
                                               i,
                                               std::ref(ctx),
                                               std::ref(ctx_mutex),
                                               std::ref(signature),
                                               std::ref(msgs),
                                               std::ref(public_keys),
                                               std::ref(public_key_map)));
        }
        for (size_t i = 0; i < set_length; i++)
        {
            results[i].get();
        }
        blst::PT gt_sig{signature};
        return ctx->finalverify(&gt_sig);
    }

    /**
     *
     *
     *
     *
     *
     *
     *
     */
    Napi::Value AggregateVerify(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        try
        {
            Napi::Array msgs_arr = InfoIndexToArray(env, info, 0, "msgs must be an array");
            Napi::Array public_keys_arr = InfoIndexToArray(env, info, 1, "publicKeys must be an array");
            size_t set_length{msgs_arr.Length()};
            if (set_length == 0)
            {
                Napi::RangeError::New(env, "msgs and publicKeys cannot be empty").ThrowAsJavaScriptException();
                return Napi::Boolean::New(env, false);
            }
            if (set_length != public_keys_arr.Length())
            {
                Napi::RangeError::New(env, "msgs[] and publicKeys[] must be the same length").ThrowAsJavaScriptException();
                return Napi::Boolean::New(env, false);
            }
            Napi::Value sig_val = info[2];
            blst::P2_Affine signature = GetSignature(env, sig_val);

            std::unique_ptr<blst::Pairing> ctx = std::unique_ptr<blst::Pairing>(new blst::Pairing(true, DST));
            std::mutex ctx_mutex;
            std::vector<ByteArray> msgs(set_length);
            std::vector<ByteArray> public_keys(set_length);
            std::map<size_t, PublicKey *> public_key_map;

            ParseMessagesAndKeys(env,
                                 set_length,
                                 msgs_arr,
                                 public_keys_arr,
                                 msgs,
                                 public_keys,
                                 public_key_map);
            // bool result = AggregateSync(
            //     set_length,
            //     ctx,
            //     signature,
            //     msgs,
            //     public_keys,
            //     public_key_map);
            size_t thread_count = 10;
            bool result = RunAggregation(
                set_length,
                ctx,
                ctx_mutex,
                signature,
                msgs,
                public_keys,
                public_key_map,
                thread_count);
            return Napi::Boolean::New(env, result);
        }
        catch (const Napi::Error &err)
        {
            err.ThrowAsJavaScriptException();
            return Napi::Boolean::New(env, false);
        }
        catch (const blst::BLST_ERROR &err)
        {
            Napi::Error::New(env, get_blst_error_string(err)).ThrowAsJavaScriptException();
            return Napi::Boolean::New(env, false);
        }
        catch (...)
        {
            std::exception_ptr err = std::current_exception();
            Napi::Error::New(env, stringify_error_ptr(err)).ThrowAsJavaScriptException();
            return Napi::Boolean::New(env, false);
        }
    }
    /**
     *
     *
     *
     *
     *
     *
     *
     */
    class AggregateVerifyWorker : public Napi::AsyncWorker
    {
    public:
        AggregateVerifyWorker(const Napi::Env &env);
        void Setup(const Napi::CallbackInfo &info);
        void Execute() override;
        void OnOK() override;
        void OnError(const Napi::Error &err) override;
        Napi::Promise GetPromise();

    private:
        Napi::Promise::Deferred deferred;
        std::unique_ptr<blst::Pairing> ctx;
        std::mutex ctx_mutex;
        size_t set_length;
        size_t thread_count;
        blst::P2_Affine signature;
        std::vector<ByteArray> msgs;
        std::vector<ByteArray> public_keys;
        std::map<size_t, PublicKey *> public_key_map;
        bool result;
    };

    AggregateVerifyWorker::AggregateVerifyWorker(const Napi::Env &env)
        : Napi::AsyncWorker{env},
          deferred{env},
          ctx{new blst::Pairing(true, DST)},
          ctx_mutex{},
          set_length{0},
          thread_count{},
          signature{},
          msgs{},
          public_keys{},
          public_key_map{},
          result{false}
    {
    }

    void AggregateVerifyWorker::Setup(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        Napi::Array msgs_arr = InfoIndexToArray(env, info, 0, "msgs must be an array");
        Napi::Array public_keys_arr = InfoIndexToArray(env, info, 1, "publicKeys must be an array");
        size_t set_length{msgs_arr.Length()};
        if (set_length == 0)
        {
            throw Napi::RangeError::New(env, "msgs and publicKeys cannot be empty");
        }
        if (set_length != public_keys_arr.Length())
        {
            throw Napi::RangeError::New(env, "msgs[] and publicKeys[] must be the same length");
        }
        Napi::Value sig_val = info[2];
        signature = GetSignature(env, sig_val);
        ParseMessagesAndKeys(env,
                             set_length,
                             msgs_arr,
                             public_keys_arr,
                             msgs,
                             public_keys,
                             public_key_map);
        Queue();
    }

    void AggregateVerifyWorker::Execute()
    {
        try
        {
            RunAggregation(
                set_length,
                ctx,
                ctx_mutex,
                signature,
                msgs,
                public_keys,
                public_key_map,
                thread_count);
            result = ctx->finalverify();
        }
        catch (blst::BLST_ERROR err)
        {
            std::ostringstream errorMsg;
            errorMsg << "BLST_ERROR:: " << get_blst_error_string(err) << " in verifyMultipleAggregateSignatures";
            SetError(errorMsg.str());
            return;
        }
    }

    void AggregateVerifyWorker::OnOK()
    {
        deferred.Resolve(Napi::Boolean::New(Env(), result));
    }

    void AggregateVerifyWorker::OnError(Napi::Error const &err)
    {
        deferred.Reject(err.Value());
    }

    Napi::Promise AggregateVerifyWorker::GetPromise()
    {
        return deferred.Promise();
    }

    Napi::Value AggregateVerifyAsync(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        try
        {
            AggregateVerifyWorker *worker = new AggregateVerifyWorker{env};
            worker->Setup(info);
            return worker->GetPromise();
        }
        catch (const Napi::Error &err)
        {
            err.ThrowAsJavaScriptException();
            return Napi::Boolean::New(env, false);
        }
        catch (...)
        {
            std::exception_ptr err = std::current_exception();
            Napi::Error::New(env, stringify_error_ptr(err)).ThrowAsJavaScriptException();
            return Napi::Boolean::New(env, false);
        }
    }
} // namespace agg_verify