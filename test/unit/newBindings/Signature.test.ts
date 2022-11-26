import {expect} from "chai";
import {Signature} from "../../../src/lib/bindings";
import {expectHex} from "../../utils";
import {signatureExample} from "./__fixtures__";

describe("Signature", () => {
  it("should exist", () => {
    expect(Signature).to.exist;
  });
  describe("constructor", () => {
    it("should have a private new Signature()", () => {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-call, @typescript-eslint/no-unsafe-return
      expect(() => new (Signature as any)()).to.throw(
        "new Signature() is a private constructor. Use static Signature.fromBytes()"
      );
    });
    describe("Signature.fromBytes()", () => {
      it("should only take Uint8Array or Buffer", () => {
        expect(() => Signature.fromBytes(3 as any)).to.throw("sigBytes must be a Uint8Array or Buffer");
      });
      it("should only take 48 or 96 bytes", () => {
        expect(() => Signature.fromBytes(Buffer.alloc(32, "*"))).to.throw("sigBytes must be 96 or 192 bytes long");
      });
      it("should take uncompressed byte arrays", () => {
        expectHex(Signature.fromBytes(signatureExample.p2).serialize(), signatureExample.p2);
      });
      it("should take compressed byte arrays", () => {
        expectHex(Signature.fromBytes(signatureExample.p2Comp).serialize(), signatureExample.p2);
      });
      it("should throw on invalid key", () => {
        const pkSeed = Signature.fromBytes(signatureExample.p2Comp);
        expect(() =>
          Signature.fromBytes(Uint8Array.from([...pkSeed.serialize().subarray(0, 94), ...Buffer.from("a1")]))
        ).to.throw("BLST_BAD_ENCODING");
      });
    });
    describe("methods", () => {
      it("should have a sigValidate()", () => {
        const sig = Signature.fromBytes(signatureExample.p2Comp);
        expect(sig.sigValidate()).to.be.undefined;
      });
    });
  });
});
