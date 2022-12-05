#include <vector>
#include <memory.h>
#include "napi.h"
#include "blst.hpp"
#include "../public_key.hpp"
#include "../blst_ts_utils.hpp"

class AggPubKeysWorker : public Napi::AsyncWorker
{
private:
    Napi::Promise::Deferred deferred;
    Napi::Reference<Napi::Array> arrayReference;
    size_t keys_length;
    std::vector<ByteArray> keys;
    std::vector<blst::P1> points;
    blst::P1 point;

public:
    AggPubKeysWorker(Napi::Env env, Napi::Array &publicKeys);
    ~AggPubKeysWorker();
    void Execute() override;
    void OnOK() override;
    void OnError(Napi::Error const &err) override;
    Napi::Promise GetPromise();
};

Napi::Value AggregatePublicKeys(const Napi::CallbackInfo &info)
{
    auto env = info.Env();
    auto val0 = info[0];
    if (!val0.IsArray())
    {
        Napi::TypeError::New(env, "aggregatePublicKeys() takes and array").ThrowAsJavaScriptException();
        return env.Undefined();
    }
    auto keysArray = info[0].As<Napi::Array>();
    AggPubKeysWorker *worker = new AggPubKeysWorker{env, keysArray};
    worker->Queue();
    return worker->GetPromise();
}

AggPubKeysWorker::AggPubKeysWorker(Napi::Env env, Napi::Array &publicKeys)
    : Napi::AsyncWorker{env},
      deferred{env},
      arrayReference{Napi::Reference<Napi::Array>::New(publicKeys, 1)},
      keys_length{publicKeys.Length()},
      keys{},
      points{},
      point{}
{
    keys.reserve(keys_length);
    points.reserve(keys_length);
    for (size_t i = 0; i < keys_length; i++)
    {
        keys.push_back(ByteArray{env, publicKeys[i], true});
    }
}

AggPubKeysWorker::~AggPubKeysWorker()
{
    auto res = arrayReference.Unref();
    if (res > 0)
    {
        printf("AggPubKeysWorker::arrayReference.Unref() returned non-zero ref count");
        // Napi::TypeError::New(Env(), "arrayReference.Unref() returned no zero refs").ThrowAsJavaScriptException();
    }
}

void AggPubKeysWorker::Execute()
{
    for (size_t i = 0; i < keys_length; i++)
    {
        try
        {
            // points[i] = blst::P1_Affine{keys[i].Data(), keys[i].ByteLength()};
        }
        catch (blst::BLST_ERROR err)
        {
            std::string msg{"Invalid key at index :"};
            msg.append(std::to_string(i));
            this->SetError(msg);
            return;
        }
        try
        {
            // point.add(points[i]);
        }
        catch (blst::BLST_ERROR err)
        {
            std::string msg{"Invalid point addition at index :"};
            msg.append(std::to_string(i));
            this->SetError(msg);
            return;
        }
    }
}

void AggPubKeysWorker::OnOK()
{
    deferred.Resolve(PublicKey::Create(Env(), &point, nullptr));
}

void AggPubKeysWorker::OnError(Napi::Error const &err)
{
    deferred.Reject(err.Value());
}

Napi::Promise AggPubKeysWorker::GetPromise()
{
    return deferred.Promise();
}
