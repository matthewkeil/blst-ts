#include <sstream>
#include <future>
#include "napi.h"
#include "../blst_ts_utils.hpp"
#include "../utils/thread_pool.h"

#ifndef RANDOM_BYTES_LENGTH
#define RANDOM_BYTES_LENGTH 8
#endif

class VerifyMultipleAggregateSignaturesWorker : public Napi::AsyncWorker
{
private:
    Napi::Promise::Deferred deferred;
    std::unique_ptr<blst::Pairing> ctx;
    std::mutex ctx_mutex;
    size_t set_length;
    std::vector<ByteArray> msgs;
    std::vector<ByteArray> publicKeys;
    std::vector<ByteArray> signatures;
    bool result;
    void Aggregate(size_t i);

public:
    VerifyMultipleAggregateSignaturesWorker(Napi::Env env, Napi::Array &signatureSets);
    void Execute() override;
    void OnOK() override;
    void OnError(Napi::Error const &err) override;
    Napi::Promise GetPromise();
};

Napi::Value VerifyMultipleAggregateSignatures(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    auto val0 = info[0];
    if (!val0.IsArray())
    {
        Napi::TypeError::New(env, "verifyMultipleAggregateSignatures() takes and array").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    auto keysArray = info[0].As<Napi::Array>();
    VerifyMultipleAggregateSignaturesWorker *worker = new VerifyMultipleAggregateSignaturesWorker{env, keysArray};
    worker->Queue();
    return worker->GetPromise();
}

VerifyMultipleAggregateSignaturesWorker::VerifyMultipleAggregateSignaturesWorker(Napi::Env env, Napi::Array &signatureSets)
    : Napi::AsyncWorker{env},
      deferred{env},
      ctx{new blst::Pairing(true, DST)},
      ctx_mutex{},
      set_length{signatureSets.Length()},
      msgs{},
      publicKeys{},
      signatures{},
      result{false}
{
    msgs.resize(set_length);
    publicKeys.resize(set_length);
    signatures.resize(set_length);
    for (size_t i = 0; i < set_length; i++)
    {
        Napi::Value val = signatureSets[i];
        if (!val.IsObject())
        {
            Napi::Error::New(env, "SignatureSet must be an object").ThrowAsJavaScriptException();
            return;
        }
        Napi::Object set = val.As<Napi::Object>();
        if (!set.Has("msg"))
        {
            Napi::Error::New(env, "SignatureSet must have a 'msg' property").ThrowAsJavaScriptException();
            return;
        }
        Napi::Value msgValue = set.Get("msg");
        if (!msgValue.IsTypedArray())
        {
            Napi::Error::New(env, "SignatureSet.msg must be a Uint8Array").ThrowAsJavaScriptException();
            return;
        }
        msgs[i] = ByteArray{msgValue.As<Napi::Uint8Array>()};
        if (!set.Has("publicKey"))
        {
            Napi::Error::New(env, "SignatureSet must have a 'publicKey' property").ThrowAsJavaScriptException();
            return;
        }
        Napi::Value publicKeyValue = set.Get("publicKey");
        if (!publicKeyValue.IsTypedArray())
        {
            Napi::Error::New(env, "SignatureSet.publicKey must be a Uint8Array").ThrowAsJavaScriptException();
            return;
        }
        publicKeys[i] = ByteArray{publicKeyValue.As<Napi::Uint8Array>()};
        if (!set.Has("signature"))
        {
            Napi::Error::New(env, "SignatureSet must have a 'signature' property").ThrowAsJavaScriptException();
            return;
        }
        Napi::Value signatureValue = set.Get("signature");
        if (!signatureValue.IsTypedArray())
        {
            Napi::Error::New(env, "SignatureSet.signature must be a Uint8Array").ThrowAsJavaScriptException();
            return;
        }
        signatures[i] = ByteArray{signatureValue.As<Napi::Uint8Array>()};
    }
}

void aggregate(
    std::vector<ByteArray> &msgs,
    std::vector<ByteArray> &publicKeys,
    std::vector<ByteArray> &signatures,
    size_t i,
    std::mutex &ctx_mutex,
    std::unique_ptr<blst::Pairing> &ctx)
{
    blst::Pairing *thread_ctx = new blst::Pairing(true, DST);
    blst::P1_Affine pk{publicKeys[i].Data(), publicKeys[i].ByteLength()};
    blst::P2_Affine sig{signatures[i].Data(), signatures[i].ByteLength()};
    blst::BLST_ERROR err = thread_ctx->mul_n_aggregate(&pk,
                                                       &sig,
                                                       ByteArray::RandomBytes(RANDOM_BYTES_LENGTH).Data(),
                                                       RANDOM_BYTES_LENGTH,
                                                       msgs[i].Data(),
                                                       msgs[i].ByteLength());
    if (err != blst::BLST_ERROR::BLST_SUCCESS)
    {
        throw err;
    }
    thread_ctx->commit();
    std::lock_guard<std::mutex> guard{ctx_mutex};
    ctx->merge(thread_ctx);
    delete thread_ctx;
}

void VerifyMultipleAggregateSignaturesWorker::Execute()
{
    try
    {
        ThreadPool pool{10};
        std::future<void> results[set_length];
        for (size_t i = 0; i < set_length; i++)
        {
            results[i] = pool.submit(std::bind(&aggregate,
                                               std::ref(msgs),
                                               std::ref(publicKeys),
                                               std::ref(signatures),
                                               i,
                                               std::ref(ctx_mutex),
                                               std::ref(ctx)));
        }

        for (size_t i = 0; i < set_length; i++)
        {
            results[i].get();
        }

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

void VerifyMultipleAggregateSignaturesWorker::OnOK()
{
    deferred.Resolve(Napi::Boolean::New(Env(), result));
}

void VerifyMultipleAggregateSignaturesWorker::OnError(Napi::Error const &err)
{
    deferred.Reject(err.Value());
}

Napi::Promise VerifyMultipleAggregateSignaturesWorker::GetPromise()
{
    return deferred.Promise();
}
