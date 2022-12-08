import {expect} from "chai";
import {functions, PublicKey} from "../../../src/lib/bindings";
import {getBindingTestSets} from "./__fixtures__";

describe("aggregatePublicKeys()", () => {
  const sets = getBindingTestSets(10);
  const keys = sets.map((set) => set.napi.publicKey.serialize());
  it("should return the promise of a PublicKey", async () => {
    const aggPromise = functions.aggregatePublicKeys(keys);
    expect(aggPromise).to.be.instanceOf(Promise);
    const agg = await aggPromise;
    expect(agg).to.be.instanceOf(PublicKey);
  });
  it("should be able to keyValidate PublicKey", async () => {
    const agg = await functions.aggregatePublicKeys(keys);
    expect(agg.keyValidate()).to.be.undefined;
  });
});
