import {expect} from "chai";
import {PublicKey, SecretKey, Signature} from "../../../src/lib/bindings";
import {SecretKey as SkType} from "../../../src/lib/bindings.types";
import {KEY_MATERIAL, SECRET_KEY_BYTES} from "./__fixtures__";

describe("SecretKey", () => {
  it("should exist", () => {
    expect(SecretKey).to.exist;
  });
  describe("constructors", () => {
    describe("new SecretKey()", () => {
      it("should not take any arguments", () => {
        // eslint-disable-next-line @typescript-eslint/no-unsafe-call, @typescript-eslint/no-unsafe-return
        expect(() => new (SecretKey as any)({foo: "bar"})).to.throw(
          "No arguments are allowed in SecretKey constructor"
        );
      });
      it("should create an instance", () => {
        const sk = new SecretKey();
        expect(sk).to.be.instanceOf(SecretKey);
      });
    });
    describe("SecretKey.keygen", () => {
      it("should create an instance", () => {
        expect(SecretKey.keygen()).to.be.instanceOf(SecretKey);
      });
      it("should throw on non-Uint8Array ikm", () => {
        expect(() => SecretKey.keygen(null as any)).to.throw("IKM for new SecretKey(ikm) must be a Uint8Array");
        expect(() => SecretKey.keygen(42 as any)).to.throw("IKM for new SecretKey(ikm) must be a Uint8Array");
        expect(() => SecretKey.keygen("Uint8Array" as any)).to.throw("IKM for new SecretKey(ikm) must be a Uint8Array");
        expect(() => SecretKey.keygen({} as any)).to.throw("IKM for new SecretKey(ikm) must be a Uint8Array");
        expect(() => SecretKey.keygen([] as any)).to.throw("IKM for new SecretKey(ikm) must be a Uint8Array");
      });
      it("should take UintArray8 for ikm", () => {
        expect(SecretKey.keygen(KEY_MATERIAL)).to.be.instanceOf(SecretKey);
      });
      it("should create the same key from the same ikm", () => {
        expect(SecretKey.keygen(KEY_MATERIAL).toBytes().toString()).to.equal(
          SecretKey.keygen(KEY_MATERIAL).toBytes().toString()
        );
      });
    });
    describe("SecretKey.fromBytes", () => {
      it("should create an instance", () => {
        expect(SecretKey.fromBytes(SECRET_KEY_BYTES)).to.be.instanceOf(SecretKey);
      });
    });
  });
  describe("methods", () => {
    let key: SkType;
    beforeEach(() => {
      key = SecretKey.keygen();
    });
    describe("toBytes", () => {
      it("should serialize the key to Uint8Array", () => {
        expect(key.toBytes()).to.be.instanceof(Uint8Array);
      });
      it("should be 32 bytes long", () => {
        expect(key.toBytes().length).to.equal(32);
      });
      it("should reconstruct the same key", () => {
        const serialized = key.toBytes();
        expect(SecretKey.fromBytes(serialized).toBytes().toString()).to.equal(serialized.toString());
      });
    });
    describe("getPublicKey", () => {
      it("should create a PublicKey", () => {
        expect(new SecretKey().getPublicKey()).to.be.instanceOf(PublicKey);
      });
    });
    describe("sign", () => {
      it("should create a Signature", () => {
        const sig = new SecretKey().sign(Buffer.from("some fancy message"));
        expect(sig).to.be.instanceOf(Signature);
        expect(sig.sigValidate()).to.be.undefined;
      });
    });
  });
});
