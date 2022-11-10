#include <thread>
#include "napi.h"
#include "blst.hpp"
#include "utils.h"

const size_t RAND_BYTES = 8;

typedef struct NapiSignatureSet
{
    Napi::Uint8Array msg;
    Napi::Uint8Array publicKey;
    Napi::Uint8Array signature;
};

typedef struct SignatureSet
{
    char *msg;
    uint8_t msgLength;
    blst::P1_Affine publicKey;
    blst::P2_Affine signature;
};

class VerifyMultipleAggregateSignatureWorker : public Napi::AsyncWorker
{
public:
    Napi::Promise::Deferred deferred;

    VerifyMultipleAggregateSignatureWorker(Napi::Env env, Napi::Array &signatureSets)
        : Napi::AsyncWorker(env), signatureSets(signatureSets), deferred(env)
    {
    }

    ~VerifyMultipleAggregateSignatureWorker() {}
    // This code will be executed on the worker thread
    void Execute() override
    {
        const std::string dst = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";
        blst::Pairing *ctx = new blst::Pairing(true, dst);

        const uint32_t signatureSetsLength = signatureSets.Length();
        for (uint32_t i = 0; i < signatureSetsLength; i++)
        {
            Napi::Value element = signatureSets[i];
            if (!element.IsObject())
            {
                this->SetError("signatureSets must be an array of objects");
                return;
            }

            Napi::Object set = element.As<Napi::Object>();
            if (!set.Has("msg") || !set.Get("msg").IsString())
            {
                this->deferred.Reject(Napi::Error::New(Env(), "SignatureSet.msg must be a string").Value());
                return;
            }
            if (!set.Has("publicKey") || !set.Get("publicKey").IsArrayBuffer())
            {
                this->SetError("SignatureSet.publicKey must be an ArrayBuffer");
                return;
            }
            if (!set.Has("signature") || !set.Get("signature").IsArrayBuffer())
            {
                this->SetError("SignatureSet.signature must be an ArrayBuffer");
                return;
            }

            size_t publicKeyLength = set.Get("publicKey").As<Napi::ArrayBuffer>().ByteLength();
            blst::byte *publicKey = static_cast<uint8_t *>(set.Get("publicKey").As<Napi::ArrayBuffer>().Data());
            blst::P1_Affine pk = blst::P1_Affine(publicKey, publicKeyLength);
            size_t signatureLength = set.Get("signature").As<Napi::ArrayBuffer>().ByteLength();
            uint8_t *signature = static_cast<uint8_t *>(set.Get("signature").As<Napi::ArrayBuffer>().Data());
            blst::P2_Affine sig = blst::P2_Affine(signature, signatureLength);
            uint8_t rand[RAND_BYTES];
            randomBytesNonZero(rand, RAND_BYTES);
            std::string msg = set.Get("msg").As<Napi::String>().Utf8Value();

            blst::BLST_ERROR err = ctx->mul_n_aggregate(&pk, &sig, rand, RAND_BYTES, msg);
            if (err != blst::BLST_SUCCESS)
            {
                this->SetError(get_blst_error_string(err));
                return;
            }
        }

        ctx->commit();
        bool result = ctx->finalverify();
        if (!result)
        {
            this->SetError("invalid finalVerify");
            return;
        }
    }

    void OnOK() override
    {
        this->deferred.Resolve(Napi::Boolean::New(Env(), true));
    }

    void OnError(Napi::Error const &err) override
    {
        this->deferred.Reject(err.Value());
    }

private:
    Napi::Array signatureSets;
};
