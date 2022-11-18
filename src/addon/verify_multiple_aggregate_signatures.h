#ifndef __BLST_TS_VERIFY_MULTIPLE_AGGREGATE_SIGNATURES_H__
#define __BLST_TS_VERIFY_MULTIPLE_AGGREGATE_SIGNATURES_H__

#include <sodium.h>
#include <vector>
#include <string>
#include "blst_ts.hpp"
// #include "blst_ts_utils.hpp"

typedef struct
{
    std::vector<uint8_t> msg;
    uint8_t publicKey[96];
    uint8_t signature[192];
} SignatureSet;

blst::BLST_ERROR verify_multiple_aggregate_signatures(std::vector<SignatureSet> &sets, bool &result);

#endif /* __BLST_TS_VERIFY_MULTIPLE_AGGREGATE_SIGNATURES_H__ */