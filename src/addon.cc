#include "addon.h"

std::mutex GlobalState::_lock;

GlobalState::GlobalState()
    : dst_{"BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_"},
      _secret_key_length{32},
      _public_key_compressed_length{48},
      _public_key_uncompressed_length{96},
      _signature_compressed_length{96},
      _signature_uncompressed_length{192},
      // TODO: Should these be on addon with a uuid?
      _secret_key_type{"BLST_TS_SECRET_KEY"},
      _public_key_type{"BLST_TS_PUBLIC_KEY"},
      _signature_type{"BLST_TS_SIGNATURE"},
      _blst_error_strings{
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

std::shared_ptr<GlobalState> GlobalState::GetInstance(BlstTsAddon *addon)
{
    static std::weak_ptr<GlobalState> shared;
    const std::lock_guard<std::mutex> guard(_lock);
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

void BlstTsAddon::BuildJsConstants(Napi::Env &env)
{
    _js_constants = Napi::Object::New(env);
    _js_constants.Set(Napi::String::New(env, "DST"), Napi::String::New(env, _global_state->dst_));
    _js_constants.Set(Napi::String::New(env, "SECRET_KEY_LENGTH"), Napi::Number::New(env, _global_state->_secret_key_length));
    _js_constants.Set(Napi::String::New(env, "PUBLIC_KEY_LENGTH_UNCOMPRESSED"), Napi::Number::New(env, _global_state->_public_key_uncompressed_length));
    _js_constants.Set(Napi::String::New(env, "PUBLIC_KEY_LENGTH_COMPRESSED"), Napi::Number::New(env, _global_state->_public_key_compressed_length));
    _js_constants.Set(Napi::String::New(env, "SIGNATURE_LENGTH_UNCOMPRESSED"), Napi::Number::New(env, _global_state->_signature_uncompressed_length));
    _js_constants.Set(Napi::String::New(env, "SIGNATURE_LENGTH_COMPRESSED"), Napi::Number::New(env, _global_state->_signature_compressed_length));
}

std::string BlstTsAddon::GetBlstErrorString(blst::BLST_ERROR &err)
{
    return _global_state->_blst_error_strings[err];
}

BlstTsAddon::BlstTsAddon(Napi::Env env, Napi::Object exports)
{
    env.SetInstanceData(this);
    SecretKey::Init(env, exports, this);
    PublicKey::Init(env, exports, this);
    Signature::Init(env, exports, this);
    BuildJsConstants(env);
    DefineAddon(exports, {
                             InstanceValue("BLST_CONSTANTS", _js_constants, napi_enumerable),
                         });
};

NODE_API_ADDON(BlstTsAddon)