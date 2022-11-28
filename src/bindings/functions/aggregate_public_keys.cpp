#include "aggregate_public_keys.hpp"

AggPubKeysWorker::AggPubKeysWorker(Napi::Env env, Napi::Array &publicKeys)
    : Napi::AsyncWorker{env},
      deferred{env},
      arrayReference{Napi::Reference<Napi::Array>::New(publicKeys, 1)},
      keys_length{publicKeys.Length()},
      keys{keys_length},
      points{keys_length},
      point{}
{
    for (uint32_t i = 0; i < keys_length; i++)
    {
        Napi::Value key = publicKeys[i];
        if (key.IsString())
        {
            auto str = key.As<Napi::String>().Utf8Value();
            keys[i].type = KeyType::string;
            keys[i].ptr = (char *)str.c_str();
            keys[i].len = str.size();
        }
        else if (key.IsTypedArray())
        {
            auto buf = key.As<Napi::TypedArrayOf<uint8_t>>();
            keys[i].type = KeyType::bytes;
            keys[i].ptr = buf.Data();
            keys[i].len = buf.ByteLength();
        }
        else
        {
            Napi::TypeError::New(env, "aggregatePublicKeys(keys) keys must be an array of hex string or a Uint8Array").ThrowAsJavaScriptException();
        }
    }
}

AggPubKeysWorker::~AggPubKeysWorker()
{
    arrayReference.Unref();
}

void AggPubKeysWorker::Execute()
{
    for (size_t i = 0; i < keys_length; i++)
    {
        try
        {
            if (keys[i].type == KeyType::string)
            {
                size_t key_len = keys[i].len;
                blst::byte key_buff[key_len / 2];
                hex_string_to_bytes(key_buff, key_len, (char *)keys[i].ptr, key_len);
                points[i] = blst::P1{key_buff, key_len};
            }
            else
            {
                points[i] = blst::P1{(uint8_t *)keys[i].ptr, keys[i].len};
            }
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
            point.add(points[i]);
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

Napi::Value aggregatePublicKeys(const Napi::CallbackInfo &info)
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