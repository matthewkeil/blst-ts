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

class GlobalState
{
public:
    std::string dst_;
    uint8_t secret_key_length_;
    uint8_t public_key_compressed_length_;
    uint8_t public_key_uncompressed_length_;
    uint8_t signature_compressed_length_;
    uint8_t signature_uncompressed_length_;
    std::string secret_key_type_;
    std::string public_key_type_;
    std::string signature_type_;
    std::string blst_error_strings_[8];

    GlobalState();
    GlobalState(GlobalState &&source) = delete;
    GlobalState(const GlobalState &source) = delete;
    GlobalState &operator=(GlobalState &&source) = delete;
    GlobalState &operator=(const GlobalState &source) = delete;

    Napi::Object GetJSConstants();

    static std::shared_ptr<GlobalState> GetInstance(BlstTsAddon *addon);

private:
    static std::mutex lock_;
    std::vector<BlstTsAddon *> instances;
};

class BlstTsAddon : public Napi::Addon<BlstTsAddon>
{
public:
    std::shared_ptr<GlobalState> global_state_ = GlobalState::GetInstance(this);
    Napi::Object js_constants_;
    Napi::Function secret_key_ctr_;
    Napi::Function public_key_ctr_;
    Napi::Function signature_ctr_;

    BlstTsAddon(Napi::Env env, Napi::Object exports);
    BlstTsAddon(BlstTsAddon &&source);
    BlstTsAddon(const BlstTsAddon &source) = default;
    BlstTsAddon &operator=(BlstTsAddon &&source);
    BlstTsAddon &operator=(const BlstTsAddon &source) = default;

private:
    void BuildConstants(Napi::Env env);
};

#endif /* BLST_TS_ADDON_H__ */