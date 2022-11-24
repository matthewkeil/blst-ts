import {expect} from "chai";
import {PublicKey} from "../../../src/lib/bindings";

describe("PublicKey", () => {
  it("should exist", () => {
    expect(PublicKey).to.exist;
  });
  describe("constructor", () => {
    it("should be private", () => {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-call, @typescript-eslint/no-unsafe-return
      expect(() => new (PublicKey as any)("some string")).to.throw(
        "new PublicKey() takes a SecretKey instance. Use static PublicKey.fromBytes()"
      );
    });
  });
});
