#include <vector>
#include <string>
#include <napi.h>
#include "blst.hpp"
#include "../public_key.hpp"
#include "../blst_ts_utils.hpp"

blst::BLST_ERROR aggregate_public_keys(
    blst::P1 &out,
    std::vector<blst::P1> &keys);

blst::BLST_ERROR aggregate_public_keys(
    blst::P1 &out,
    blst::byte **keys,
    size_t key_length,
    size_t keys_length);

enum KeyType
{
    string,
    bytes
};

typedef struct
{
    KeyType type;
    void *ptr;
    size_t len;
} KeyMeta;

class AggPubKeysWorker : public Napi::AsyncWorker
{
private:
    Napi::Promise::Deferred deferred;
    Napi::Reference<Napi::Array> arrayReference;
    size_t keys_length;
    std::vector<KeyMeta> keys;
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

Napi::Value aggregatePublicKeys(const Napi::CallbackInfo &info);