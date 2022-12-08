#include <sstream>
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
    size_t keys_length;
    std::vector<ByteArray> keys;
    std::vector<blst::P1> points;
    blst::P1 point;

public:
    AggPubKeysWorker(Napi::Env env, Napi::Array &publicKeys);
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
      keys_length{publicKeys.Length()},
      keys{},
      points{},
      point{}
{
    keys.reserve(keys_length);
    for (size_t i = 0; i < keys_length; i++)
    {
        Napi::Value val = publicKeys[i];
        if (val.IsString())
        {
            keys.push_back(ByteArray{val.As<Napi::String>().Utf8Value()});
        }
        else if (val.IsTypedArray() || val.IsBuffer())
        {
            keys.push_back(ByteArray{val.As<Napi::TypedArrayOf<uint8_t>>()});
        }
        else
        {
            Napi::TypeError::New(env, "ByteArray::FromValue supports utf-8 string | Buffer | Uint8Array").ThrowAsJavaScriptException();
        }
    }
}

void AggPubKeysWorker::Execute()
{
    points.reserve(keys_length);
    for (size_t i = 0; i < keys_length; i++)
    {
        try
        {
            points[i] = blst::P1{keys[i].Data(), keys[i].ByteLength()};
        }
        catch (blst::BLST_ERROR err)
        {
            std::ostringstream msg;
            msg << "Error " << get_blst_error_string(err) << ": Invalid key at index " << i;
            this->SetError(msg.str());
            return;
        }
        try
        {
            point.add(points[i]);
        }
        catch (blst::BLST_ERROR err)
        {
            std::ostringstream msg;
            msg << "Error " << get_blst_error_string(err) << ": Invalid key aggregation at index " << i;
            this->SetError(msg.str());
            return;
        }
    }
    keys.clear();
    points.clear();
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
