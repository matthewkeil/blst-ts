// import * as swigBindings from "../../deprecated/src/swig/lib";
// import * as napiBindings from "../../src";

// type SetType = "swig" | "napi";
// interface KeyPair {
//   ikm: Uint8Array;
//   secretKey: napiBindings.SecretKey;
//   publicKey: napiBindings.PublicKey;
// }
// interface NapiTestSet extends KeyPair {
//   msg: Uint8Array;
//   signature: napiBindings.Signature;
// }
// interface SwigTestSetUnmapped {
//   ikm: Uint8Array;
//   msg: Uint8Array;
//   secretKey: swigBindings.SecretKey;
//   publicKey: swigBindings.PublicKey;
//   signature: swigBindings.Signature;
// }
// interface SwigTestSet {
//   ikm: Uint8Array;
//   msg: Uint8Array;
//   sk: swigBindings.SecretKey;
//   pk: swigBindings.PublicKey;
//   sig: swigBindings.Signature;
// }

// // Create and cache (on demand) crypto data to benchmark
// const napiSets = new Map<number, NapiTestSet>();
// const swigSets = new Map<number, SwigTestSetUnmapped>();
// const napiKeyPairs = new Map<number, KeyPair>();
// const swigKeyPairs = new Map<number, KeyPair>();

// function getKeyPair(i: number, type: SetType): KeyPair {
//   const isNapi = type === "napi";
//   const keyPairs = isNapi ? napiKeyPairs : swigKeyPairs;
//   let keyPair = keyPairs.get(i);
//   if (!keyPair) {
//     const bindings = isNapi ? napiBindings : swigBindings;
//     const ikm = Buffer.alloc(32, i + 1);
//     const secretKey = bindings.SecretKey.fromKeygen(ikm);
//     const publicKey = secretKey.toPublicKey();
//     keyPair = {ikm, secretKey, publicKey} as KeyPair;
//     keyPairs.set(i, keyPair);
//   }
//   return keyPair;
// }

// function getSet<T extends SetType>(i: number, type: T): T extends "napi" ? NapiTestSet : SwigTestSetUnmapped {
//   const isNapi = type === "napi";
//   const sets = isNapi ? napiSets : swigSets;
//   let set = sets.get(i);
//   if (!set) {
//     const {ikm, secretKey, publicKey} = getKeyPair(i, type);
//     const msg = Buffer.alloc(32, i + 1);
//     set = {ikm, msg, secretKey, publicKey, signature: secretKey.sign(msg)};
//     sets.set(i, set as any);
//   }
//   return set as T extends "napi" ? NapiTestSet : SwigTestSetUnmapped;
// }

// export const getNapiSet = (i: number): NapiTestSet => getSet(i, "napi");
// export const getSwigSet = (i: number): SwigTestSet => {
//   const set = getSet(i, "swig");
//   return {
//     ikm: set.ikm,
//     sk: set.secretKey,
//     pk: set.publicKey,
//     msg: set.msg,
//     sig: set.signature,
//   };
// };
// export const getNapiSecretKey = (i: number): SecretKey => {
//   const set = getSet(i, "napi");
//   return set.secretKey as SecretKey;
// };
// export const getSwigSecretKey = (i: number): swigBindings.SecretKey => {
//   const set = getSet(i, "swig");
//   return set.secretKey;
// };
// export const getNapiPublicKey = (i: number): PublicKey => {
//   const set = getSet(i, "napi");
//   return set.publicKey as PublicKey;
// };
// export const getSwigPublicKey = (i: number): swigBindings.PublicKey => {
//   const set = getSet(i, "swig");
//   return set.publicKey as swigBindings.PublicKey;
// };
// export const getNapiSignature = (i: number): Signature => {
//   const set = getSet(i, "napi");
//   return set.signature as Signature;
// };
// export const getSwigSignature = (i: number): swigBindings.Signature => {
//   const set = getSet(i, "swig");
//   return set.signature as swigBindings.Signature;
// };
