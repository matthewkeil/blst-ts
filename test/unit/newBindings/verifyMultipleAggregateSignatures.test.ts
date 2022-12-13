import {expect} from "chai";
import {functions} from "../../../src/lib/bindings";
import {getBindingTestSets} from "../../__fixtures__";

describe("verifyMultipleAggregateSignaturesWorker()", () => {
  it("should exist", () => {
    expect(typeof functions.verifyMultipleAggregateSignatures).to.equal("function");
  });
  it("should return a Promise<boolean>", async () => {
    const resPromise = functions.verifyMultipleAggregateSignatures([]);
    expect(resPromise).to.be.instanceOf(Promise);
    const result = await resPromise;
    expect(typeof result).to.equal("boolean");
  });
  it("should default to false", async () => {
    const result = await functions.verifyMultipleAggregateSignatures([]);
    expect(result).to.be.false;
  });
  it("should return true for valid sets", async () => {
    const sets = getBindingTestSets(10).map((set) => {
      const {msg, swig} = set;
      const {publicKey, signature} = swig;
      return {
        msg,
        publicKey: publicKey.serialize(),
        signature: signature.serialize(),
      };
    });
    const res = await functions.verifyMultipleAggregateSignatures(sets);
    expect(res).to.be.true;
  });
});
