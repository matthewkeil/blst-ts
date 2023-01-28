import {expect} from "chai";
import {functions} from "../../src/lib/bindings";
import {PublicKey, Signature} from "../../src/lib/bindings.types";
import {makeNapiTestSets} from "../utils";

describe("aggregateVerify", () => {
  let signature: Signature;
  const sets = makeNapiTestSets(10).map((set) => {
    const {msg, secretKey, publicKey} = set;
    signature = signature ?? set.signature;
    return {
      msg,
      secretKey,
      publicKey,
      signature,
    };
  });

  const asBuffers = sets.map(({msg, secretKey, publicKey, signature}) => ({
    msg: msg,
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
  const invalidSet = makeNapiTestSets(1)[0];
  const badKey = asStrings[0].publicKey.slice(-6).concat("000000");
  const badSignature = asStrings[0].signature.slice(-6).concat("000000");
  const selectMsg = ({msg}: any): Uint8Array => msg as Uint8Array;
  const selectPublicKey = ({publicKey}: any): PublicKey => publicKey as PublicKey;

  describe("Inputs", () => {
    // describe("Invalid inputs", () => {
    //   it("should throw for not passing an array", () => {});
    //   it("should throw for not passing an array of objects", () => {});
    //   it("each SignatureSet.msg exists", () => {});
    //   it("each SignatureSet.publicKey exists", () => {});
    //   it("each SignatureSet.signature exists", () => {});
    // });
    // it("should take hex string inputs", () => {
    //   const msgs = asStrings.map(({msg}) => msg);
    //   const publicKeys = asStrings.map(({publicKey}) => publicKey);
    //   const result = functions.aggregateVerify(msgs, publicKeys, signature);
    //   expect(result).to.be.true;
    // });
    // it("should take Uint8Array inputs", () => {
    //   const msgs = asBuffers.map(selectMsg);
    //   const publicKeys = asBuffers.map(selectPublicKey);
    //   const result = functions.aggregateVerify(msgs, publicKeys, signature);
    //   expect(result).to.be.true;
    // });
  });
  describe("Synchronous", () => {
    const msgs = asBuffers.map(selectMsg);
    const publicKeys = asBuffers.map(selectPublicKey);
    it("should exist", () => {
      expect(typeof functions.aggregateVerify).to.equal("function");
    });
    it("should return a boolean", () => {
      const result = functions.aggregateVerify(msgs, publicKeys, signature);
      expect(typeof result).to.equal("boolean");
    });
    it("should default to false", () => {
      const result = functions.aggregateVerify(msgs, publicKeys, invalidSet.signature);
      expect(result).to.be.false;
    });
    // it("should return true for valid sets", () => {
    //   const res = functions.aggregateVerify(msgs, publicKeys, signature);
    //   expect(res).to.be.true;
    // });
  });
  // describe("Promise Based", () => {
  //   it("should exist", () => {
  //     expect(typeof functions.verifyMultipleAggregateSignatures).to.equal("function");
  //   });
  //   it("should return a Promise<boolean>", async () => {
  //     const resPromise = functions.verifyMultipleAggregateSignaturesAsync([]);
  //     expect(resPromise).to.be.instanceOf(Promise);
  //     const result = await resPromise;
  //     expect(typeof result).to.equal("boolean");
  //   });
  //   it("should default to false", async () => {
  //     const result = await functions.verifyMultipleAggregateSignaturesAsync([]);
  //     expect(result).to.be.false;
  //   });
  //   it("should return true for valid sets", async () => {
  //     const res = await functions.verifyMultipleAggregateSignaturesAsync(sets);
  //     expect(res).to.be.true;
  //   });
  // });
});
