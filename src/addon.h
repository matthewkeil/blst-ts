#ifndef BLST_TS_ADDON_H__
#define BLST_TS_ADDON_H__

#ifdef DEBUG
#define DEBUG_HEADER fprintf(stderr, "blst-ts [%s:%s() %d]: ", __FILE__, __FUNCTION__, __LINE__);
#define DEBUG_FOOTER fprintf(stderr, "\n");
#define DEBUG_LOG(...)                         \
    DEBUG_HEADER fprintf(stderr, __VA_ARGS__); \
    DEBUG_FOOTER
#else
#define DEBUG_LOG(...)
#endif

#include <mutex>
#include <memory>
#include <napi.h>

class BlstTsAddon;

#include "secret_key.h"

class GlobalState
{
public:
    std::string dst_;
    size_t _secret_key_length;
    size_t _public_key_compressed_length;
    size_t _public_key_uncompressed_length;
    size_t _signature_compressed_length;
    size_t _signature_uncompressed_length;
    std::string _secret_key_type;
    std::string public_key_type_;
    std::string signature_type_;
    std::string _blst_error_strings[8];

    GlobalState();
    GlobalState(GlobalState &&source) = delete;
    GlobalState(const GlobalState &source) = delete;
    GlobalState &operator=(GlobalState &&source) = delete;
    GlobalState &operator=(const GlobalState &source) = delete;

    static std::shared_ptr<GlobalState> GetInstance(BlstTsAddon *addon);

private:
    static std::mutex _lock;
};

class BlstTsAddon : public Napi::Addon<BlstTsAddon>
{
public:
    std::shared_ptr<GlobalState> _global_state = GlobalState::GetInstance(this);
    Napi::Object _js_constants;
    Napi::FunctionReference _secret_key_ctr;
    Napi::Function _public_key_ctr;
    Napi::Function _signature_ctr;

    BlstTsAddon(Napi::Env env, Napi::Object exports);
    BlstTsAddon(BlstTsAddon &&source) = delete;
    BlstTsAddon(const BlstTsAddon &source) = delete;
    BlstTsAddon &operator=(BlstTsAddon &&source) = delete;
    BlstTsAddon &operator=(const BlstTsAddon &source) = delete;

private:
    void BuildJsConstants(Napi::Env &env);
};

#endif /* BLST_TS_ADDON_H__ */