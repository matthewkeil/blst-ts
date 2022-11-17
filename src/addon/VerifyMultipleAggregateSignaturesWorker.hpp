#include <vector>
#include <napi.h>
#include <sodium.h>
#include "blst.hpp"

const size_t RANDOM_BYTES_LENGTH = 8;

typedef struct
{
    std::vector<uint8_t> msg;
    uint8_t publicKey[96];
    uint8_t signature[192];
} SignatureSet;

class VerifyMultipleAggregateSignaturesWorker : public Napi::AsyncWorker
{
private:
    Napi::Promise::Deferred deferred;
    bool result;
    std::vector<SignatureSet> sets;

public:
    VerifyMultipleAggregateSignaturesWorker(Napi::Env env, Napi::Array &signatureSets)
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

    void Execute() override
    {
        const std::string dst = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";
        blst::Pairing ctx = blst::Pairing(true, dst);

        if (sodium_init() < 0)
        {
            this->SetError("Error initializing libsodium");
            return;
        }

        for (size_t i = 0; i < sets.size(); i++)
        {
            blst::P1_Affine pk = blst::P1_Affine{sets[i].publicKey};
            blst::P2_Affine sig = blst::P2_Affine{sets[i].signature};
            blst::byte scalar[RANDOM_BYTES_LENGTH];
            randombytes_buf(scalar, RANDOM_BYTES_LENGTH);
            const std::string_view msg{reinterpret_cast<char *>(sets[i].msg.data()), sets[i].msg.size()};
            blst::BLST_ERROR err = ctx.mul_n_aggregate(&pk, &sig, scalar, RANDOM_BYTES_LENGTH, msg);
            if (err != blst::BLST_SUCCESS)
            {
                this->SetError(get_blst_error_string(err));
                return;
            }
        }

        printf("after loop\n");

        ctx.commit();
        printf("after commit\n");
        result = ctx.finalverify();
        printf("after finalverify\n");
        printf("result: %d", result);
        return;
    }

    void OnOK() override
    {
        deferred.Resolve(Napi::Boolean::New(Env(), result));
    }

    void OnError(Napi::Error const &err) override
    {
        deferred.Reject(err.Value());
    }

    Napi::Promise GetPromise()
    {
        return this->deferred.Promise();
    }
};
