#ifndef BLST_TS_ADDON_H__
#define BLST_TS_ADDON_H__

#include <napi.h>
#include "secret_key.h"

class GlobalState
{
public:
    uint8_t secret_key_length_;
    uint8_t public_key_compressed_length_;
    uint8_t public_key_uncompressed_length_;
    uint8_t signature_compressed_length_;
    uint8_t signature_uncompressed_length_;
    std::string secret_key_type_;
    std::string public_key_type_;
    std::string signature_type_;
    std::string dst_;
    std::string blst_error_strings_[8];

    GlobalState::GlobalState();
    GlobalState(GlobalState &&source) = delete;
    GlobalState(const GlobalState &source) = delete;
    GlobalState &operator=(GlobalState &&source) = delete;
    GlobalState &operator=(const GlobalState &source) = delete;

    static std::shared_ptr<GlobalState> GetInstance();

private:
    static std::mutex lock_;
};

class BlstTsAddon : public Napi::Addon<BlstTsAddon>
{
public:
    std::shared_ptr<GlobalState> global_state_ = GlobalState::GetInstance();
    Napi::Function secret_key_ctr_;

    BlstTsAddon(Napi::Env env, Napi::Object exports);
};

#endif /* BLST_TS_ADDON_H__ */