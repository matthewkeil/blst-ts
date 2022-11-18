#include "vmas_worker.h"

static const char *get_blst_error_string(blst::BLST_ERROR err)
{
    return blst::BLST_ERROR_STRINGS[err];
};

VerifyMultipleAggregateSignaturesWorker::VerifyMultipleAggregateSignaturesWorker(Napi::Env env, Napi::Array &signatureSets)
    : Napi::AsyncWorker(env), deferred{Napi::Promise::Deferred::New(env)}, result{false}, sets{0}
{
    for (size_t i = 0; i < signatureSets.Length(); i++)
    {
        Napi::Value val = signatureSets[i];
        if (!val.IsObject())
        {
            Napi::Error err = Napi::Error::New(env, "SignatureSet must be an object");
            err.ThrowAsJavaScriptException();
            return;
        }
        Napi::Object set = val.ToObject();

        sets.push_back(SignatureSet());

        if (!set.Has("msg"))
        {
            Napi::Error err = Napi::Error::New(env, "SignatureSet must have a 'msg' property");
            err.ThrowAsJavaScriptException();
            return;
        }
        Napi::Value msgValue = set.Get("msg");
        if (!msgValue.IsTypedArray())
        {
            Napi::Error err = Napi::Error::New(env, "SignatureSet.msg must be a Uint8Array");
            err.ThrowAsJavaScriptException();
            return;
        }
        Napi::Uint8Array msgArray = set.Get("msg").As<Napi::Uint8Array>();
        size_t msgLength = msgArray.ByteLength();
        sets[i].msg.resize(msgLength);
        uint8_t *msgData = msgArray.Data();
        sets[i].msg.insert(sets[i].msg.end(), msgData[0], msgData[msgLength]);

        if (!set.Has("publicKey"))
        {
            Napi::Error err = Napi::Error::New(env, "SignatureSet must have a 'publicKey' property");
            err.ThrowAsJavaScriptException();
            return;
        }
        Napi::Value publicKeyValue = set.Get("publicKey");
        if (!publicKeyValue.IsTypedArray())
        {
            Napi::Error err = Napi::Error::New(env, "SignatureSet.publicKey must be a Uint8Array");
            err.ThrowAsJavaScriptException();
            return;
        }
        Napi::Uint8Array publicKeyArray = set.Get("publicKey").As<Napi::Uint8Array>();
        size_t publicKeyLength = publicKeyArray.ByteLength();
        memcpy(&sets[i].publicKey, publicKeyArray.Data(), publicKeyLength < 96 ? publicKeyLength : 96);

        if (!set.Has("signature"))
        {
            Napi::Error err = Napi::Error::New(env, "SignatureSet must have a 'signature' property");
            err.ThrowAsJavaScriptException();
            return;
        }
        Napi::Value signatureValue = set.Get("signature");
        if (!signatureValue.IsTypedArray())
        {
            Napi::Error err = Napi::Error::New(env, "SignatureSet.signature must be a Uint8Array");
            err.ThrowAsJavaScriptException();
            return;
        }
        Napi::Uint8Array signatureArray = set.Get("signature").As<Napi::Uint8Array>();
        size_t signatureLength = signatureArray.ByteLength();
        memcpy(&sets[i].signature, signatureArray.Data(), signatureLength < 192 ? signatureLength : 192);
    }
}

void VerifyMultipleAggregateSignaturesWorker::Execute()
{

    blst::BLST_ERROR err = verify_multiple_aggregate_signatures(this->sets, this->result);
    if (err > 0)
    {
        this->SetError(get_blst_error_string(err));
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
    return this->deferred.Promise();
}
