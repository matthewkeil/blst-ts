import {PublicKey, SecretKey, aggregatePublicKeys} from "../../../src/lib/bindings";
import {SecretKey as SK, PublicKey as PK} from "../../../src/lib/bindings.types";

interface KeySet {
  sk: SK;
  pk: PK;
  bytes: Uint8Array;
  string: string;
}
describe("aggregatePublicKeys", () => {
  const sets: KeySet[] = [];
  sets.length = 10;
  sets.fill({} as any);
  for (const set of sets) {
    set.sk = SecretKey.keygen();
    set.pk = new PublicKey(set.sk);
    set.bytes = set.pk.toBytes();
    set.string = Buffer.from(set.bytes).toString("hex");
  }

  //   console.log(sets);
  it("should work", async () => {
    const agg = await aggregatePublicKeys(sets.map((set) => set.bytes));
    console.log(agg.serialize());
  });
});
