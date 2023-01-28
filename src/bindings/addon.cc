#include <napi.h>
#include "addon.h"
#include "secret_key.h"

GlobalState::GlobalState() : secret_key_length_{32},
                             public_key_compressed_length_{48},
                             public_key_uncompressed_length_{96},
                             signature_compressed_length_{96},
                             signature_uncompressed_length_{192},
                             // TODO: Should these be on addon with a uuid?
                             secret_key_type_{"BLST_TS_SECRET_KEY"},
                             public_key_type_{"BLST_TS_PUBLIC_KEY"},
                             signature_type_{"BLST_TS_SIGNATURE"},
                             dst_{"BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"},
                             blst_error_strings_{
                                 "BLST_SUCCESS",
                                 "BLST_BAD_ENCODING",
                                 "BLST_POINT_NOT_ON_CURVE",
                                 "BLST_POINT_NOT_IN_GROUP",
                                 "BLST_AGGR_TYPE_MISMATCH",
                                 "BLST_VERIFY_FAIL",
                                 "BLST_PK_IS_INFINITY",
                                 "BLST_BAD_SCALAR",
                             }
{
}

std::shared_ptr<GlobalState> GlobalState::GetInstance()
{
    static std::weak_ptr<GlobalState> shared;
    std::lock_guard<std::mutex> guard(lock_);

    // Get an existing instance from the weak reference, if possible.
    if (auto instance = shared.lock())
    {
        return instance;
    }

    // Create a new instance and keep a weak reference.
    // Global state will be cleaned up when last thread exits.
    auto instance = std::make_shared<GlobalState>();
    shared = instance;
    return instance;
}

BlstTsAddon::BlstTsAddon(Napi::Env env, Napi::Object exports)
{
    SecretKey::Init(env, exports, secret_key_ctr_, this);
}

NODE_API_ADDON(BlstTsAddon)