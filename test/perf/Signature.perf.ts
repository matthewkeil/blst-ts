import {itBench} from "@dapplion/benchmark";
import * as swigBindings from "../../src/swig/lib";
import napiBindings from "../../src/lib/bindings";
import {getSwigSet, getNapiSet, getSwigSignature, getNapiSignature} from "./perf.helpers";

describe("Signature", () => {
  describe("constructors", () => {
    describe("fromBytes()", () => {
      itBench({
        id: "Signature.fromBytes() - swig implementation",
        beforeEach: () => getSwigSignature(Math.floor(Math.random() * 10)).toBytes(),
        fn: (bytes) => void swigBindings.Signature.fromBytes(bytes),
      });
      itBench({
        id: "Signature.fromBytes() - napi implementation",
        beforeEach: () => getNapiSignature(Math.floor(Math.random() * 10)).toBytes(),
        fn: (bytes) => void napiBindings.Signature.fromBytes(bytes),
      });
    });
  });
  describe("methods", () => {
    describe("toBytes()", () => {
      itBench({
        id: "signature.toBytes() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).sig,
        fn: (sig) => void sig.toBytes(),
      });
      itBench({
        id: "signature.toBytes() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).signature,
        fn: (sig) => void sig.toBytes(),
      });
    });
    describe("compress()", () => {
      itBench({
        id: "signature.compress() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).sig,
        fn: (sig) => void sig.compress(),
      });
      itBench({
        id: "signature.compress() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).signature,
        fn: (sig) => void sig.compress(),
      });
    });
    describe("serialize()", () => {
      itBench({
        id: "signature.serialize() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).sig,
        fn: (sig) => void sig.serialize(),
      });
      itBench({
        id: "signature.serialize() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).signature,
        fn: (sig) => void sig.serialize(),
      });
    });
    describe("sigValidate()", () => {
      itBench({
        id: "signature.sigValidate() - swig implementation",
        beforeEach: () => getSwigSet(Math.floor(Math.random() * 10)).sig,
        fn: (sig) => void sig.sigValidate(),
      });
      itBench({
        id: "signature.sigValidate() - napi implementation",
        beforeEach: () => getNapiSet(Math.floor(Math.random() * 10)).signature,
        fn: (sig) => void sig.sigValidate(),
      });
    });
  });
});
