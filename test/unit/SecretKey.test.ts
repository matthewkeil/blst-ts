import {expect} from "chai";
import {PublicKey, SecretKey, Signature} from "../../lib";
import {KEY_MATERIAL, SECRET_KEY_BYTES} from "../__fixtures__";

describe("SecretKey", () => {
  it("should exist", () => {
    expect(SecretKey).to.exist;
  });
  it("should create an instance", () => {
    const sk = SecretKey.fromKeygenSync();
    expect(sk).to.be.instanceOf(SecretKey);
  });
  describe("constructors", () => {
    describe("new SecretKey()", () => {
      it("should have a private constructor", () => {
        // eslint-disable-next-line @typescript-eslint/no-unsafe-call, @typescript-eslint/no-unsafe-return
        expect(() => new (SecretKey as any)("foo-bar-baz")).to.throw("SecretKey constructor is private");
      });
    });
    describe("SecretKey.fromKeygen", () => {
      it("should create an instance", () => {
        expect(SecretKey.fromKeygenSync()).to.be.instanceOf(SecretKey);
      });
      it("should throw on non-Uint8Array ikm", () => {
        /* eslint-disable quotes */
        expect(() => SecretKey.fromKeygenSync(null as any)).to.throw('"ikm" must be a Uint8Array');
        expect(() => SecretKey.fromKeygenSync(42 as any)).to.throw('"ikm" must be a Uint8Array');
        expect(() => SecretKey.fromKeygenSync("Uint8Array" as any)).to.throw('"ikm" must be a Uint8Array');
        expect(() => SecretKey.fromKeygenSync({} as any)).to.throw('"ikm" must be a Uint8Array');
        expect(() => SecretKey.fromKeygenSync([] as any)).to.throw('"ikm" must be a Uint8Array');
        /* eslint-enable quotes */
      });
      it("should take UintArray8 for ikm", () => {
        expect(SecretKey.fromKeygenSync(KEY_MATERIAL)).to.be.instanceOf(SecretKey);
      });
      it("should create the same key from the same ikm", () => {
        expect(SecretKey.fromKeygenSync(KEY_MATERIAL).serialize().toString()).to.equal(
          SecretKey.fromKeygenSync(KEY_MATERIAL).serialize().toString()
        );
      });
    });
    describe("SecretKey.fromBytes", () => {
      it("should create an instance", () => {
        expect(SecretKey.deserialize(SECRET_KEY_BYTES)).to.be.instanceOf(SecretKey);
      });
    });
  });
  describe("methods", () => {
    let key: SecretKey;
    beforeEach(() => {
      key = SecretKey.fromKeygenSync();
    });
    describe("serialize", () => {
      it("should serialize the key to Uint8Array", () => {
        expect(key.serialize()).to.be.instanceof(Uint8Array);
      });
      it("should be 32 bytes long", () => {
        expect(key.serialize().length).to.equal(32);
      });
      it("should reconstruct the same key", () => {
        const serialized = key.serialize();
        expect(SecretKey.deserialize(serialized).serialize().toString()).to.equal(serialized.toString());
      });
    });
    describe("toPublicKey", () => {
      it("should create a PublicKey", () => {
        expect(SecretKey.fromKeygenSync().toPublicKey()).to.be.instanceOf(PublicKey);
      });
    });
    describe("sign", () => {
      it("should create a Signature", () => {
        const sig = SecretKey.fromKeygenSync().signSync(Buffer.from("some fancy message"));
        expect(sig).to.be.instanceOf(Signature);
        expect(sig.sigValidateSync()).to.be.undefined;
      });
    });
  });
});
