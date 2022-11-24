import {expect} from "chai";
import {PublicKey, SecretKey} from "../../../src/lib/bindings";
import type {SecretKey as SkType} from "../../../src/lib/bindings.types";
import {getFilledUint8} from "../../utils";

const ikm = getFilledUint8(32);
const skBytes = Uint8Array.from(Buffer.from("5620799c63c92bb7912122070f7ebb6ddd53bdf9aa63e7a7bffc177f03d14f68", "hex"));

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
        expect(SecretKey.keygen(ikm)).to.be.instanceOf(SecretKey);
      });
    });
    describe("SecretKey.fromBytes", () => {
      it("should create an instance", () => {
        expect(SecretKey.fromBytes(skBytes)).to.be.instanceOf(SecretKey);
      });
    });
  });
  describe("methods", () => {
    let key: SkType;
    beforeEach(() => {
      key = SecretKey.keygen();
    });
    // describe("getPublicKey", () => {});
    // describe("sign", () => {});
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
    describe("toPublicKey", () => {
      it("should create a PublicKey", () => {
        expect(new SecretKey().getPublicKey()).to.be.instanceOf(PublicKey);
      });
    });
  });
});
