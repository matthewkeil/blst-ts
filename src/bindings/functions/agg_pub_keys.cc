#include <sstream>
#include <vector>
#include <map>
#include <memory.h>
#include "napi.h"
#include "blst.hpp"
#include "../public_key.hpp"
#include "../blst_ts_utils.hpp"

namespace agg_pub_keys
{

    void AggregatePublicKeys(
        blst::P1 &point,
        bool &has_error,
        std::string &error_msg,
        size_t keys_length,
        std::vector<ByteArray> &public_keys,
        std::map<size_t, PublicKey *> &public_key_map) noexcept
    {
        has_error = false;
        blst::P1 current;
        for (size_t i = 0; i < keys_length; i++)
        {
            blst::P1_Affine current;
            try
            {
                auto pk_iter = public_key_map.find(i);
                if (pk_iter != public_key_map.end())
                {
                    current = pk_iter->second->AsAffine();
                }
                else
                {
                    current = blst::P1_Affine{public_keys[i].Data(), public_keys[i].ByteLength()};
                }
            }
            catch (blst::BLST_ERROR err)
            {
                std::ostringstream msg;
                msg << "BLST_ERROR::" << get_blst_error_string(err) << ": Invalid key at index " << i;
                error_msg = msg.str();
                has_error = true;
                return;
            }
            try
            {
                point.add(current);
            }
            catch (blst::BLST_ERROR err)
            {
                std::ostringstream msg;
                msg << "BLST_ERROR::" << get_blst_error_string(err) << ": Invalid key aggregation at index " << i;
                error_msg = msg.str();
                has_error = true;
            }
        }
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
            public_keys[index] = bytes;
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

    Napi::Value AggregatePublicKeys(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        Napi::Array public_keys_arr = InfoIndexToArray(env, info, 0, "aggregatePublicKeys() takes and array");
        size_t keys_length = public_keys_arr.Length();
        std::vector<ByteArray> public_keys(keys_length);
        std::map<size_t, PublicKey *> public_key_map;
        for (size_t i = 0; i < keys_length; i++)
        {
            GetPublicKey(env, i, public_keys_arr, public_keys, public_key_map);
        }
        blst::P1 *point = new blst::P1;
        bool has_error = false;
        std::string error_msg;
        AggregatePublicKeys(*point, has_error, error_msg, keys_length, public_keys, public_key_map);
        if (has_error)
        {
            Napi::Error::New(env, error_msg).ThrowAsJavaScriptException();
        }
        return PublicKey::Create(env, point, nullptr);
    }

    class AggPubKeysWorker : public Napi::AsyncWorker
    {
    public:
        AggPubKeysWorker(const Napi::Env &env, const Napi::Array &public_keys_arr);
        void Setup(const Napi::Env &env, const Napi::Array &public_keys_arr);
        void Execute() override;
        void OnOK() override;
        void OnError(Napi::Error const &err) override;
        Napi::Promise GetPromise();

    private:
        Napi::Promise::Deferred deferred;
        blst::P1 point;
        size_t keys_length;
        std::vector<ByteArray> public_keys;
        std::map<size_t, PublicKey *> public_key_map;
    };

    Napi::Value AggregatePublicKeysAsync(const Napi::CallbackInfo &info)
    {
        Napi::Env env = info.Env();
        Napi::Array public_keys_arr = InfoIndexToArray(env, info, 0, "aggregatePublicKeys() takes and array");
        AggPubKeysWorker *worker = new AggPubKeysWorker{env, public_keys_arr};
        worker->Setup(env, public_keys_arr);
        return worker->GetPromise();
    }

    AggPubKeysWorker::AggPubKeysWorker(const Napi::Env &env, const Napi::Array &public_keys_arr)
        : Napi::AsyncWorker{env},
          deferred{env},
          point{},
          keys_length{public_keys_arr.Length()},
          public_keys(keys_length),
          public_key_map{}
    {
    }

    void AggPubKeysWorker::Setup(const Napi::Env &env, const Napi::Array &public_keys_arr)
    {
        for (size_t i = 0; i < keys_length; i++)
        {
            GetPublicKey(env, i, public_keys_arr, public_keys, public_key_map);
        }
        Queue();
    }

    void AggPubKeysWorker::Execute()
    {
        bool has_error = false;
        std::string error_msg;
        AggregatePublicKeys(point, has_error, error_msg, keys_length, public_keys, public_key_map);
        if (has_error)
        {
            this->SetError(error_msg);
        }
        public_keys.clear();
        public_key_map.clear();
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
} /* namespace agg_pub_keys */