import {functions} from "../../../src/lib/bindings";
import {getBindingTestSets} from "./__fixtures__";

// interface KeySet {
//   ikm: Uint8Array;
//   sk: SK;
//   skBytes: Uint8Array;
//   skHexString: string;
//   pk: PK;
//   pkBytes: Uint8Array;
//   pkHexString: string;
// }
describe("aggregatePublicKeys", () => {
  const sets = getBindingTestSets(10);

  // const sets = new Array<KeySet>(10);
  // for (let i = 0; i < sets.length; i++) {
  //   const set = (sets[i] = {} as KeySet);
  //   set.ikm = randomBytesNonZero(32);
  //   set.sk = SecretKey.keygen(set.ikm);
  //   set.skBytes = set.sk.toBytes();
  //   set.skHexString = toHex(set.skBytes);
  //   set.pk = new PublicKey(set.sk);
  //   set.pkBytes = set.pk.toBytes();
  //   set.pkHexString = toHex(set.pkBytes);
  // }

  it("should work", async () => {
    const keys = sets.map((set) => set.napi.publicKey.serialize());
    const agg = await functions.aggregatePublicKeys(keys);
    // console.log(agg.serialize());
  });
});
