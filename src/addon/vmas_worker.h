#ifndef __BLST_TS_VMAS_WORKER_H__
#define __BLST_TS_VMAS_WORKER_H__

#include <vector>
#include <napi.h>
#include <sodium.h>
#include "blst_ts.hpp"
#include "verify_multiple_aggregate_signatures.h"

class VerifyMultipleAggregateSignaturesWorker : public Napi::AsyncWorker
{
private:
    Napi::Promise::Deferred deferred;
    bool result;
    std::vector<SignatureSet> sets;

public:
    VerifyMultipleAggregateSignaturesWorker(Napi::Env env, Napi::Array &signatureSets);
    void Execute() override;
    void OnOK() override;
    void OnError(Napi::Error const &err) override;
    Napi::Promise GetPromise();
};

#endif