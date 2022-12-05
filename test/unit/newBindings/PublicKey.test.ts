import {expect} from "chai";
import {PublicKey, SecretKey} from "../../../src/lib/bindings";
import {expectHex} from "../../utils";
import {publicKeyExample, SECRET_KEY_BYTES} from "./__fixtures__";

describe("PublicKey", () => {
  it("should exist", () => {
    expect(PublicKey).to.exist;
  });
  describe("constructors", () => {
    describe("new PublicKey()", () => {
      it("should only take an object", () => {
        // eslint-disable-next-line @typescript-eslint/no-unsafe-call, @typescript-eslint/no-unsafe-return
        expect(() => new (PublicKey as any)("some string")).to.throw("new PublicKey() takes an object");
      });
      it("should only take a SecretKey object", () => {
        // eslint-disable-next-line @typescript-eslint/no-unsafe-call, @typescript-eslint/no-unsafe-return
        expect(() => new (PublicKey as any)({})).to.throw("Must pass a SecretKey to new PublicKey()");
        expect(() => new PublicKey({__type: "Signature"} as any)).to.throw("Must pass a SecretKey to new PublicKey()");
        // When overloading the private __type get error from SecretKey::Unwrap
        expect(() => new PublicKey({__type: "SecretKey"} as any)).to.throw("Invalid argument");
      });
      it("should return the same PublicKey from the same SecretKey", () => {
        const sk = SecretKey.fromBytes(SECRET_KEY_BYTES);
        const pk1 = new PublicKey(sk);
        const pk2 = new PublicKey(sk);
        expect(pk1.serialize().toString()).to.equal(pk2.serialize().toString());
      });
    });
    describe("fromBytes", () => {
      it("should only take Uint8Array or Buffer", () => {
        expect(() => PublicKey.fromBytes(3 as any)).to.throw("pkBytes must be a Uint8Array or Buffer");
      });
      it("should only take 48 or 96 bytes", () => {
        expect(() => PublicKey.fromBytes(Buffer.alloc(32, "*"))).to.throw("pkBytes must be 48 or 96 bytes long");
      });
      it("should take uncompressed byte arrays", () => {
        expectHex(PublicKey.fromBytes(publicKeyExample.p1).serialize(), publicKeyExample.p1);
      });
      it("should take compressed byte arrays", () => {
        expectHex(PublicKey.fromBytes(publicKeyExample.p1Comp).serialize(), publicKeyExample.p1);
      });
      it("should throw on invalid key", () => {
        const pkSeed = PublicKey.fromBytes(publicKeyExample.p1Comp);
        expect(() =>
          PublicKey.fromBytes(Uint8Array.from([...pkSeed.serialize().subarray(0, 94), ...Buffer.from("a1")]))
        ).to.throw("BLST_POINT_NOT_ON_CURVE");
      });
    });
  });
  describe("methods", () => {
    describe("keyValidate()", () => {
      it("should not throw on valid public key", () => {
        const sk = SecretKey.fromBytes(SECRET_KEY_BYTES);
        const pk = new PublicKey(sk);
        expect(pk.keyValidate()).to.be.undefined;
      });
    });
  });
});
