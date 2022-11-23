import {expect} from "chai";
import {SecretKey} from "../../../src/lib/bindings";
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
      // it("should take UintArray8 for ikm", () => {
      //   expect(SecretKey.keygen(ikm)).to.be.instanceOf(SecretKey);
      // });
    });
    describe("SecretKey.fromBytes", () => {
      it("should create an instance", () => {
        expect(SecretKey.fromBytes(skBytes)).to.be.instanceOf(SecretKey);
      });
    });
  });
});
