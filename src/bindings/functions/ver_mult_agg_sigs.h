#ifndef BLST_TS_FUNCTIONS_VER_MULT_AGG_SIGS_H_
#define BLST_TS_FUNCTIONS_VER_MULT_AGG_SIGS_H_

#include <sstream>
#include <future>
#include <map>
#include <optional>
#include <string>
#include "napi.h"
#include "../blst_ts_utils.hpp"
#include "../public_key.hpp"
#include "../signature.hpp"
#include "../utils/thread_pool.h"

/**
 *
 *  VerifyMultipleAggregateSignatures
 *
 */
// Napi::Value GetObjectKey(
//     Napi::Env &env,
//     Napi::Object &obj,
//     std::string &key_name);

// void TryStringAndBuffer(
//     Napi::Env &env,
//     Napi::Value &value,
//     std::vector<ByteArray> &vec,
//     size_t insert_index,
//     std::string &key_name,
//     bool should_throw = false);

// template <typename T>
// T *TryAs(
//     Napi::Env &env,
//     Napi::Object &obj,
//     bool should_throw = false);

// void GetMessage(Napi::Env &env,
//                 Napi::Object &set,
//                 std::vector<ByteArray> &msgs,
//                 size_t insert_index);

// void GetPublicKey(Napi::Env &env,
//                   Napi::Object &set,
//                   size_t insert_index,
//                   std::vector<ByteArray> &publicKeys,
//                   std::map<size_t, PublicKey *> &publicKeyMap);

// void GetSignature(Napi::Env &env,
//                   Napi::Object &set,
//                   size_t insert_index,
//                   std::vector<ByteArray> &signatures,
//                   std::map<size_t, Signature *> &signatureMap);

// void ParseSignatureSets(Napi::Env &env,
//                         Napi::Array &signatureSets,
//                         size_t set_length,
//                         std::vector<ByteArray> &msgs,
//                         std::vector<ByteArray> &publicKeys,
//                         std::vector<ByteArray> &signatures,
//                         std::map<size_t, PublicKey *> &publicKeyMap,
//                         std::map<size_t, Signature *> &signatureMap,
//                         size_t insert_index);

// void aggregate(
//     ByteArray &msg,
//     ByteArray &publicKey,
//     ByteArray &signature,
//     size_t i,
//     std::mutex &ctx_mutex,
//     std::unique_ptr<blst::Pairing> &ctx);

// Napi::Value VerifyMultipleAggregateSignatures(const Napi::CallbackInfo &info);

/**
 *
 *
 *
 *
 *
 *
 *
 */
// class VerifyMultipleAggregateSignaturesWorker : public Napi::AsyncWorker
// {
// public:
//     VerifyMultipleAggregateSignaturesWorker(Napi::Env env, Napi::Array &signatureSets);
//     void Execute() override;
//     void OnOK() override;
//     void OnError(Napi::Error const &err) override;
//     Napi::Promise GetPromise();

// private:
//     Napi::Promise::Deferred deferred;
//     std::unique_ptr<blst::Pairing> ctx;
//     std::mutex ctx_mutex;
//     size_t set_length;
//     std::vector<ByteArray> msgs;
//     std::vector<ByteArray> publicKeys;
//     std::vector<ByteArray> signatures;
//     bool result;
//     void Aggregate(size_t i);
// };

// Napi::Value VerifyMultipleAggregateSignaturesAsync(const Napi::CallbackInfo &info);

#endif /* BLST_TS_FUNCTIONS_VER_MULT_AGG_SIGS_H_ */