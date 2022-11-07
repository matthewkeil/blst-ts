#pragma once

#include <napi.h>
#include "sodium.h"
#include "../../blst/bindings/blst.h"

typedef struct
{
    Napi::ArrayBuffer *PK;
    Napi::ArrayBuffer *signature;
    char *msg;
} SignatureSet;

int randomBytesNonZero(byte *out, int length);
