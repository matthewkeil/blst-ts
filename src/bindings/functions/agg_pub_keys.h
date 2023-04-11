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
    void RunKeysAggregation(
        blst::P1 &point,
        bool &has_error,
        std::string &error_msg,
        size_t keys_length,
        std::vector<ByteArray> &public_keys,
        std::map<size_t, PublicKey *> &public_key_map) noexcept;
    void GetPublicKey(
        const Napi::Env &env,
        size_t index,
        const Napi::Array &pub_keys_arr,
        std::vector<ByteArray> &public_keys,
        std::map<size_t, PublicKey *> &public_key_map);
    Napi::Value AggregatePublicKeysCore(const Napi::Env &env, const Napi::Array &public_keys_arr);
    Napi::Value AggregatePublicKeys(const Napi::CallbackInfo &info);
    Napi::Value AggregatePublicKeysAsync(const Napi::CallbackInfo &info);

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

} /* namespace agg_pub_keys */