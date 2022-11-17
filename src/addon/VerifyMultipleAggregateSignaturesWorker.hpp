#include <thread>
#include <string>
#include <vector>
#include <napi.h>
#include "blst.hpp"
#include "utils.h"

const size_t RAND_BYTES = 8;

class VerifyMultipleAggregateSignaturesWorker : public Napi::AsyncWorker
{
private:
    Napi::Promise::Deferred deferred;
    bool result;
    // std::vector<struct SignatureSet> sets;

public:
    VerifyMultipleAggregateSignaturesWorker(Napi::Env env, Napi::Array &signatureSets)
        : Napi::AsyncWorker(env), deferred(Napi::Promise::Deferred::New(env)), result(false)
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

            if (set.Has("msg"))
            {
                Napi::Error err = Napi::Error::New(env, "SignatureSet must have a 'msg' property");
                err.ThrowAsJavaScriptException();
                return;
            }

            // Napi::Value msgValue = set.Get('msg');
            // if (!msgValue.IsNumber())
            // {
            //     Napi::Error err = Napi::Error::New(env, "SignatureSet.msg must be a Buffer");
            //     err.ThrowAsJavaScriptException();
            //     return;
            // }

            // Napi::Uint8Array msgArray = set.Get("msg").As<Napi::Buffer<uint8_t>>();
            // size_t msgLength = msgArray.ByteLength();
            // blst::byte msg[msgLength];
            // memcpy(msg, msgArray, msgLength);
            //  = msgArray.Data();
            // size_t msgLength = msgArray.ByteLength();
            // if (!set.Has("msg") || !set.Get("msg").IsString())
            // {
            //     Napi::Error err = Napi::Error::New(env, "SignatureSet.msg must be a string");
            //     err.ThrowAsJavaScriptException();
            //     return;
            // }
            // if (!set.Has("publicKey") || !set.Get("publicKey").IsTypedArray())
            // {
            //     this->SetError("SignatureSet.publicKey must be an ArrayBuffer");
            //     return;
            // }
            // if (!set.Has("signature") || !set.Get("signature").IsArrayBuffer())
            // {
            //     this->SetError("SignatureSet.signature must be an ArrayBuffer");
            //     return;
            // }

            // if (element.IsObject())
            // {
            //     Napi::Object obj = element;
            // }
        }
    }

    ~VerifyMultipleAggregateSignaturesWorker() {}

    Napi::Promise GetPromise()
    {
        return this->deferred.Promise();
    }

    void Execute() override
    {
        const std::string dst = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";
        blst::Pairing ctx = blst::Pairing(true, dst);

        // const uint32_t signatureSetsLength = signatureSets.Length();
        // for (uint32_t i = 0; i < signatureSetsLength; i++)
        // {
        //     // Napi::Value element = signatureSets[i];
        //     // if (!element.IsObject())
        //     // {
        //     //     this->SetError("signatureSets must be an array of objects");
        //     //     return;
        //     // }

        //     // Napi::Object set = element.As<Napi::Object>();
        //     // if (!set.Has("msg") || !set.Get("msg").IsString())
        //     // {
        //     //     this->SetError("SignatureSet.msg must be a string");
        //     //     return;
        //     // }
        //     // if (!set.Has("publicKey") || !set.Get("publicKey").IsTypedArray())
        //     // {
        //     //     this->SetError("SignatureSet.publicKey must be an ArrayBuffer");
        //     //     return;
        //     // }
        //     // if (!set.Has("signature") || !set.Get("signature").IsArrayBuffer())
        //     // {
        //     //     this->SetError("SignatureSet.signature must be an ArrayBuffer");
        //     //     return;
        //     // }

        //     // Napi::TypedArrayOf<uint8_t> publicKeyArray = set.Get("publicKey").As<Napi::TypedArrayOf<uint8_t>>();
        //     // size_t publicKeyLength = publicKeyArray.ByteLength();
        //     // blst::byte *publicKey = publicKeyArray.Data();
        //     // blst::P1_Affine pk = blst::P1_Affine(publicKey, publicKeyLength);

        //     // Napi::TypedArrayOf<uint8_t> signatureArray = set.Get("signature").As<Napi::TypedArrayOf<uint8_t>>();
        //     // size_t signatureLength = signatureArray.ByteLength();
        //     // uint8_t *signature = signatureArray.Data();
        //     // blst::P2_Affine sig = blst::P2_Affine(signature, signatureLength);

        //     // uint8_t rand[RAND_BYTES];
        //     // randomBytesNonZero(rand, RAND_BYTES);
        //     // std::string msg = set.Get("msg").As<Napi::String>().Utf8Value();

        // blst::BLST_ERROR err = ctx.mul_n_aggregate(&pk, &sig, rand, RAND_BYTES, msg);
        //     // if (err != blst::BLST_SUCCESS)
        //     // {
        //     //     this->SetError(get_blst_error_string(err));
        //     //     return;
        //     // }
        // }

        // ctx.commit();
        // result = ctx.finalverify();
    }

    void OnOK() override
    {
        deferred.Resolve(Napi::Boolean::New(Env(), result));
    }

    void OnError(Napi::Error const &err) override
    {
        deferred.Reject(err.Value());
    }
};
