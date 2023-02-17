
#include <sstream>
#include <memory>
#include "napi.h"
#include "addon.h"
#include "utils.h"
#include "public_key.h"

namespace functions
{
    Napi::Value AggregatePublicKeys(const Napi::CallbackInfo &info);
    Napi::Value AggregatePublicKeysSync(const Napi::CallbackInfo &info);
    Napi::Value AggregateSignatures(const Napi::CallbackInfo &info);
    Napi::Value AggregateSignaturesSync(const Napi::CallbackInfo &info);
    Napi::Value Verify(const Napi::CallbackInfo &info);
    Napi::Value VerifySync(const Napi::CallbackInfo &info);
    Napi::Value AggregateVerify(const Napi::CallbackInfo &info);
    Napi::Value AggregateVerifySync(const Napi::CallbackInfo &info);
    Napi::Value FastAggregateVerify(const Napi::CallbackInfo &info);
    Napi::Value FastAggregateVerifySync(const Napi::CallbackInfo &info);
    Napi::Value VerifyMultipleAggregateSignatures(const Napi::CallbackInfo &info);
    Napi::Value VerifyMultipleAggregateSignaturesSync(const Napi::CallbackInfo &info);
    void Init(const Napi::Env &env, Napi::Object &exports, BlstTsAddon *addon);
}
