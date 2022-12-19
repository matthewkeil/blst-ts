import {itBench} from "@dapplion/benchmark";
import * as swigBindings from "../../src/swig/lib";
import napiBindings from "../../src/lib/bindings";
import {getSwigSet, getNapiSet, getSwigPublicKey, getNapiPublicKey} from "./perf.helpers";

describe("PublicKey", () => {
  describe("constructors", () => {
    describe("fromBytes()", () => {
      itBench({
        id: "PublicKey.fromBytes() - swig implementation",
        beforeEach: () => getSwigPublicKey(Math.floor(Math.random() * 10)).toBytes(),
        fn: (bytes) => void swigBindings.PublicKey.fromBytes(bytes),
      });
      itBench({
        id: "PublicKey.fromBytes() - napi implementation",
        beforeEach: () => getNapiPublicKey(Math.floor(Math.random() * 10)).toBytes(),
        fn: (bytes) => void napiBindings.PublicKey.fromBytes(bytes),
      });
    });
  });
  describe("methods", () => {
    describe("toBytes()", () => {
      itBench({
        id: "publicKey.toBytes() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).pk,
        fn: (pk) => void pk.toBytes(),
      });
      itBench({
        id: "publicKey.toBytes() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).publicKey,
        fn: (pk) => void pk.toBytes(),
      });
    });
    describe("compress()", () => {
      itBench({
        id: "publicKey.compress() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).pk,
        fn: (pk) => void pk.compress(),
      });
      itBench({
        id: "publicKey.compress() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).publicKey,
        fn: (pk) => void pk.compress(),
      });
    });
    describe("serialize()", () => {
      itBench({
        id: "publicKey.serialize() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).pk,
        fn: (pk) => void pk.serialize(),
      });
      itBench({
        id: "publicKey.serialize() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).publicKey,
        fn: (pk) => void pk.serialize(),
      });
    });
    describe("keyValidate()", () => {
      itBench({
        id: "publicKey.keyValidate() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).pk,
        fn: (pk) => void pk.keyValidate(),
      });
      itBench({
        id: "publicKey.keyValidate() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).publicKey,
        fn: (pk) => void pk.keyValidate(),
      });
    });
  });
});
