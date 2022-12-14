// import {expect} from "chai";
// import {functions, SecretKey, PublicKey} from "../../../src/lib/bindings";
// import {getBindingTestSets} from "../../__fixtures__";

// describe("aggregatePublicKeys()", () => {
//   const sets = getBindingTestSets(10);
//   const keys = sets.map(({skBytes}) => SecretKey.keygen(skBytes).serialize());
//   const badKey = Uint8Array.from(Buffer.from([...keys[0].slice(8), ...Buffer.from("0123456789abcdef", "hex")]));

//   it("should return the promise of a PublicKey", async () => {
//     const aggPromise = functions.aggregatePublicKeys(keys);
//     expect(aggPromise).to.be.instanceOf(Promise);
//     const agg = await aggPromise;
//     expect(agg).to.be.instanceOf(PublicKey);
//   });
//   it("should be able to keyValidate PublicKey", async () => {
//     const agg = await functions.aggregatePublicKeys(keys);
//     expect(agg.keyValidate()).to.be.undefined;
//   });
//   it("should throw for non-array inputs", () => {
//     expect(() => functions.aggregatePublicKeys(keys[0] as any)).to.throw("aggregatePublicKeys() takes and array");
//   });
//   it("should throw for invalid key", async () => {
//     expect(badKey.length).to.equal(96);
//     try {
//       await functions.aggregatePublicKeys(keys.concat(badKey));
//       throw new Error("function should throw");
//     } catch (err) {
//       expect((err as Error).message).to.equal("Error BLST_BAD_ENCODING: Invalid key at index 10");
//     }
//   });
// });
