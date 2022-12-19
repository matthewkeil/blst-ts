import * as swigBindings from "../../src/swig/lib";
import napiBindings from "../../src/lib/bindings";
import {SecretKey, PublicKey, Signature, SignatureSet} from "../../src/lib/bindings.types";

type KeyPair = {publicKey: PublicKey; secretKey: SecretKey};

// Create and cache (on demand) crypto data to benchmark
const napiSets = new Map<number, SignatureSet>();
const swigSets = new Map<number, SignatureSet>();
const napiKeyPairs = new Map<number, KeyPair>();
const swigKeyPairs = new Map<number, KeyPair>();

function getKeyPair(i: number, type: "swig" | "napi"): KeyPair {
  const isNapi = type === "napi";
  const keyPairs = isNapi ? napiKeyPairs : swigKeyPairs;
  let keyPair = keyPairs.get(i);
  if (!keyPair) {
    const bindings = isNapi ? napiBindings : swigBindings;
    const secretKey = bindings.SecretKey.fromBytes(Buffer.alloc(32, i + 1));
    const publicKey = secretKey.toPublicKey();
    keyPair = {secretKey, publicKey} as KeyPair;
    keyPairs.set(i, keyPair as KeyPair);
  }
  return keyPair;
}

function getSet(i: number, type: "swig" | "napi"): SignatureSet {
  const isNapi = type === "napi";
  const sets = isNapi ? napiSets : swigSets;
  let set = sets.get(i);
  if (!set) {
    const {secretKey, publicKey} = getKeyPair(i, type);
    const msg = Buffer.alloc(32, i + 1);
    set = {msg, publicKey, signature: secretKey.sign(msg)};
    sets.set(i, set);
  }
  return set;
}

export const getNapiSet = (i: number): SignatureSet => getSet(i, "napi");
export const getSwigSet = (i: number): swigBindings.SignatureSet => {
  const set = getSet(i, "swig");
  return {
    msg: set.msg as Uint8Array,
    pk: set.publicKey as swigBindings.PublicKey,
    sig: set.signature as swigBindings.Signature,
  };
};
export const getNapiPublicKey = (i: number): PublicKey => {
  const set = getSet(i, "napi");
  return set.publicKey as PublicKey;
};
export const getSwigPublicKey = (i: number): swigBindings.PublicKey => {
  const set = getSet(i, "swig");
  return set.publicKey as swigBindings.PublicKey;
};
export const getNapiSignature = (i: number): Signature => {
  const set = getSet(i, "napi");
  return set.signature as Signature;
};
export const getSwigSignature = (i: number): swigBindings.Signature => {
  const set = getSet(i, "swig");
  return set.signature as swigBindings.Signature;
};
