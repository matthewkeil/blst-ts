import {expect} from "chai";
import {functions, PublicKey} from "../../../../src/lib/bindings";
import {makeNapiTestSets} from "../../../../test/utils";

describe("aggregatePublicKeys()", () => {
  const sets = makeNapiTestSets(10);
  const keys = sets.map(({publicKey}) => publicKey);
  const badKey = Uint8Array.from(
    Buffer.from([...keys[0].serialize().slice(8), ...Buffer.from("0123456789abcdef", "hex")])
  );

  describe("synchronous api", () => {
    it("should return the promise of a PublicKey", () => {
      const agg = functions.aggregatePublicKeys(keys);
      expect(agg).to.be.instanceOf(PublicKey);
    });
    it("should be able to keyValidate PublicKey", () => {
      const agg = functions.aggregatePublicKeys(keys);
      expect(agg.keyValidate()).to.be.undefined;
    });
    it("should throw for non-array inputs", () => {
      expect(() => functions.aggregatePublicKeys(keys[0] as any)).to.throw("aggregatePublicKeys() takes and array");
    });
    it("should throw for invalid key", () => {
      expect(badKey.length).to.equal(96);
      try {
        functions.aggregatePublicKeys([...keys, badKey]);
        throw new Error("function should throw");
      } catch (err) {
        expect((err as Error).message).to.equal("BLST_ERROR::BLST_BAD_ENCODING: Invalid key at index 10");
      }
    });
  });
  describe("promise api", () => {
    it("should return the promise of a PublicKey", async () => {
      const aggPromise = functions.aggregatePublicKeysAsync(keys);
      expect(aggPromise).to.be.instanceOf(Promise);
      const agg = await aggPromise;
      expect(agg).to.be.instanceOf(PublicKey);
    });
    it("should be able to keyValidate PublicKey", async () => {
      const agg = await functions.aggregatePublicKeysAsync(keys);
      expect(agg.keyValidate()).to.be.undefined;
    });
    it("should throw for non-array inputs", () => {
      expect(() => functions.aggregatePublicKeysAsync(keys[0] as any)).to.throw(
        "aggregatePublicKeys() takes and array"
      );
    });
    it("should throw for invalid key", async () => {
      expect(badKey.length).to.equal(96);
      try {
        await functions.aggregatePublicKeysAsync([...keys, badKey]);
        throw new Error("function should throw");
      } catch (err) {
        expect((err as Error).message).to.equal("BLST_ERROR::BLST_BAD_ENCODING: Invalid key at index 10");
      }
    });
  });
});
