import {expect} from "chai";
import {functions} from "../../src/lib/bindings";
import {makeNapiTestSets} from "../utils";

describe("verifyMultipleAggregateSignatures", () => {
  const sets = makeNapiTestSets(10).map((set) => {
    const {msg, secretKey, publicKey, signature} = set;
    return {
      msg,
      secretKey,
      publicKey,
      signature,
    };
  });
  describe("Inputs", () => {
    const asBuffers = sets.map(({msg, secretKey, publicKey, signature}) => ({
      msg: Buffer.from(msg).toString("hex"),
      secretKey: secretKey.serialize(),
      publicKey: publicKey.serialize(),
      signature: signature.serialize(),
    }));
    const asStrings = sets.map(({msg, secretKey, publicKey, signature}) => ({
      msg: Buffer.from(msg).toString("hex"),
      secretKey: Buffer.from(secretKey.serialize()).toString("hex"),
      publicKey: Buffer.from(publicKey.serialize()).toString("hex"),
      signature: Buffer.from(signature.serialize()).toString("hex"),
    }));
    it("should take hex string inputs", () => {
      const result = functions.verifyMultipleAggregateSignatures(asStrings);
      expect(result).to.be.true;
    });
    it("should take Uint8Array inputs", () => {
      const result = functions.verifyMultipleAggregateSignatures(asBuffers);
      expect(result).to.be.true;
    });
    describe("Invalid inputs", () => {
      it("should throw for not passing an array", () => {});
      it("should throw for not passing an array of objects", () => {});
      it("each SignatureSet.msg exists", () => {});
      it("each SignatureSet.publicKey exists", () => {});
      it("each SignatureSet.signature exists", () => {});
    });
  });
  describe("Synchronous", () => {
    it("should exist", () => {
      expect(typeof functions.verifyMultipleAggregateSignatures).to.equal("function");
    });
    it("should return a boolean", () => {
      const result = functions.verifyMultipleAggregateSignatures([]);
      expect(typeof result).to.equal("boolean");
    });
    it("should default to false", () => {
      const result = functions.verifyMultipleAggregateSignatures([]);
      expect(result).to.be.false;
    });
    it("should return true for valid sets", () => {
      const res = functions.verifyMultipleAggregateSignatures(sets);
      expect(res).to.be.true;
    });
  });
  describe("Promise Based", () => {
    it("should exist", () => {
      expect(typeof functions.verifyMultipleAggregateSignatures).to.equal("function");
    });
    it("should return a Promise<boolean>", async () => {
      const resPromise = functions.verifyMultipleAggregateSignaturesAsync([]);
      expect(resPromise).to.be.instanceOf(Promise);
      const result = await resPromise;
      expect(typeof result).to.equal("boolean");
    });
    it("should default to false", async () => {
      const result = await functions.verifyMultipleAggregateSignaturesAsync([]);
      expect(result).to.be.false;
    });
    it("should return true for valid sets", async () => {
      const res = await functions.verifyMultipleAggregateSignaturesAsync(sets);
      expect(res).to.be.true;
    });
  });
});
