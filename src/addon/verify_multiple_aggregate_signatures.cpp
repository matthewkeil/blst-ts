#include "verify_multiple_aggregate_signatures.h"

#ifndef RANDOM_BYTES_LENGTH
#define RANDOM_BYTES_LENGTH 8
#endif

blst::BLST_ERROR verify_multiple_aggregate_signatures(std::vector<SignatureSet> &sets, bool &result)
{
    const std::string dst = "BLS_SIG_BLS12381G2_XMD:SHA-256_SSWU_RO_POP_";
    blst::Pairing ctx = blst::Pairing(true, dst);

    if (sodium_init() < 0)
    {
        return blst::BLST_ERROR::BLST_VERIFY_FAIL;
    }

    for (size_t i = 0; i < sets.size(); i++)
    {
        blst::P1_Affine pk = blst::P1_Affine{sets[i].publicKey};
        blst::P2_Affine sig = blst::P2_Affine{sets[i].signature};
        blst::byte scalar[RANDOM_BYTES_LENGTH];
        randombytes_buf(scalar, RANDOM_BYTES_LENGTH);
        const std::string_view msg{reinterpret_cast<char *>(sets[i].msg.data()), sets[i].msg.size()};
        blst::BLST_ERROR err = ctx.mul_n_aggregate(&pk, &sig, scalar, RANDOM_BYTES_LENGTH, msg);
        if (err != blst::BLST_SUCCESS)
        {
            return err;
        }
    }

    printf("after loop\n");

    ctx.commit();
    printf("after commit\n");
    result = ctx.finalverify();
    printf("after finalverify\n");
    // printf("result: %d", result);
    return blst::BLST_ERROR::BLST_SUCCESS;
}
