import {expect} from "chai";
import {Signature} from "../../../src/lib/bindings";

describe("Signature", () => {
  it("should exist", () => {
    expect(Signature).to.exist;
  });
  describe("constructor", () => {
    it("should be private", () => {
      // eslint-disable-next-line @typescript-eslint/no-unsafe-call, @typescript-eslint/no-unsafe-return
      expect(() => new (Signature as any)("")).to.throw("No arguments are allowed in Signature constructor");
    });
  });
});
