import {expect} from "chai";
import * as swigBindings from "../../../src/swig/lib";
import napiBindings from "../../src/lib/bindings";
import {CoordType} from "../../src/lib/bindings.types";
import {expectHex} from "../utils";
import {getBindingTestSets, BindingTestSet} from "../__fixtures__";

describe("validation against SWIG bindings", () => {
  let bindingSets: BindingTestSet[] = [];
  before(() => {
    bindingSets = getBindingTestSets(6);
  });
  describe("SecretKey", () => {
    it("should serialize and deserialize with SWIG constructs", () => {
      let swigKey = bindingSets[0].swig.secretKey.toBytes();
      let napiKey = napiBindings.SecretKey.fromBytes(swigKey).serialize();
      expectHex(swigKey, napiKey);

      napiKey = napiBindings.SecretKey.fromKeygen(swigBindings.randomBytesNonZero(32)).serialize();
      swigKey = swigBindings.SecretKey.fromBytes(napiKey).toBytes();
      expectHex(swigKey, napiKey);
    });
  });
  describe("PublicKey", () => {
    it("should serialize and deserialize with SWIG constructs", () => {
      let swigKey = bindingSets[0].swig.publicKey.jacobian.serialize();
      let napiKey = napiBindings.PublicKey.fromBytes(swigKey, CoordType.jacobian).serialize();
      expectHex(swigKey, napiKey);

      const sk = napiBindings.SecretKey.fromKeygen(swigBindings.randomBytesNonZero(32));
      napiKey = new napiBindings.PublicKey(sk).serialize();
      swigKey = swigBindings.PublicKey.fromBytes(napiKey, CoordType.jacobian).serialize();
      expectHex(swigKey, napiKey);
    });
  });
  describe("Signature", () => {
    // it("should serialize and deserialize with SWIG constructs", () => {
    //   for (const {napi, swig} of bindingSets) {
    //     const napiFromSwig = napiBindings.Signature.fromBytes(swig.signature.serialize());
    //     expect(() => napiFromSwig.sigValidate()).not.to.throw;
    //     const swigFromNapi = swigBindings.Signature.fromBytes(napi.signature.serialize());
    //     expect(() => swigFromNapi.sigValidate()).not.to.throw;
    //     expectHex(swigFromNapi.serialize(), napiFromSwig.serialize());
    //   }
    // });
    // it("signing the same message should create the same signature", () => {
    //   for (const {msg, napi, swig} of bindingSets) {
    //     const swigSignature = swig.secretKey.sign(msg);
    //     swigSignature.sigValidate();
    //     const napiSignature = napi.secretKey.sign(msg);
    //     napiSignature.sigValidate();
    //     expectHex(swigSignature.serialize(), napiSignature.serialize());
    //   }
    // });
    it("bindings should interchangeably validate signatures", () => {
      for (const {msg} of bindingSets) {
        const bytes = swigBindings.randomBytesNonZero(32);

        const swigSK = swigBindings.SecretKey.fromKeygen(bytes);
        const swigPK = swigSK.toPublicKey();
        const swigSig = swigSK.sign(msg);
        expect(swigBindings.verify(msg, swigPK, swigSig)).to.be.true;

        const napiSK = napiBindings.SecretKey.fromKeygen(bytes);
        const napiPK = napiSK.toPublicKey();
        const napiSig = napiSK.sign(msg);
        expect(
          swigBindings.verify(
            msg,
            swigBindings.PublicKey.fromBytes(napiPK.serialize(), swigBindings.CoordType.jacobian),
            swigBindings.Signature.fromBytes(napiSig.serialize(), swigBindings.CoordType.jacobian)
          )
        ).to.be.true;

        expectHex(swigSig.serialize(), napiSig.serialize());
      }
    });
  });
  //   describe("aggregatePublicKeys()", () => {
  // const sets = getBindingTestSets(10);
  // const napiKeys = sets.map((set) => set.napi.publicKey.serialize());
  // it("should SWIG keyValidate after napi aggregation", async () => {
  //   const napi = await napiBindings.functions.aggregatePublicKeys(napiKeys);
  //   const swig = swigBindings.PublicKey.fromBytes(napi.serialize());
  //   expect(swig.keyValidate()).to.be.undefined;
  // });
  //   });
});
