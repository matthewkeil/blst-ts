// import {itBench} from "@dapplion/benchmark";
// import * as swigBindings from "../../deprecated/src/swig/lib";
// import {SecretKey} from "../../src";
// import {getSwigSet, getNapiSet, getSwigSecretKey, getNapiSecretKey} from "./perf.helpers";

// const IKM = Buffer.alloc(32, "*");

// describe("SecretKey", () => {
//   describe("constructors", () => {
//     describe("fromKeygen()", () => {
//       itBench({
//         id: "SecretKey.fromKeygen() - swig implementation",
//         fn: () => void swigBindings.SecretKey.fromKeygen(IKM),
//       });
//       itBench({
//         id: "SecretKey.fromKeygen() - napi implementation",
//         fn: () => void SecretKey.fromKeygen(IKM),
//       });
//     });
//     describe("fromBytes()", () => {
//       itBench({
//         id: "SecretKey.fromBytes() - swig implementation",
//         beforeEach: () => getSwigSecretKey(Math.floor(Math.random() * 10)).toBytes(),
//         fn: (bytes) => void swigBindings.SecretKey.fromBytes(bytes),
//       });
//       itBench({
//         id: "SecretKey.fromBytes() - napi implementation",
//         beforeEach: () => getNapiSecretKey(Math.floor(Math.random() * 10)).toBytes(),
//         fn: (bytes) => void SecretKey.deserialize(bytes),
//       });
//     });
//   });
//   describe("methods", () => {
//     describe("toBytes()", () => {
//       itBench({
//         id: "secretKey.toBytes() - swig implementation",
//         beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).sk,
//         fn: (sk) => void sk.toBytes(),
//       });
//       itBench({
//         id: "secretKey.toBytes() - napi implementation",
//         beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).secretKey,
//         fn: (sk) => void sk.toBytes(),
//       });
//     });
//     describe("toPublicKey()", () => {
//       itBench({
//         id: "secretKey.toPublicKey() - swig implementation",
//         beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).sk,
//         fn: (sk) => void sk.toPublicKey(),
//       });
//       itBench({
//         id: "secretKey.toPublicKey() - napi implementation",
//         beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).secretKey,
//         fn: (sk) => void sk.toPublicKey(),
//       });
//     });
//     describe("sign()", () => {
//       itBench({
//         id: "secretKey.sign() - swig implementation",
//         beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)),
//         fn: ({sk, msg}) => void sk.sign(msg),
//       });
//       itBench({
//         id: "secretKey.sign() - napi implementation",
//         beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)),
//         fn: ({secretKey, msg}) => void secretKey.sign(msg),
//       });
//     });
//   });
// });
