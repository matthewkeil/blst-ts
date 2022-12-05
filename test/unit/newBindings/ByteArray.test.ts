// import {expect} from "chai";
// import bindings from "../../../src/lib/bindings";
// import {expectHex, getFilledUint8} from "../../utils";

// const {
//   testBufferAsBuffer,
//   testBufferAsString,
//   testStringAsBuffer,
//   testTypedArrayAsString,
//   testTypedArrayAsTypedArray,
// } = bindings.functions.tests;

// describe("ByteArray", () => {
//   it("should take buffer and return as buffer", () => {
//     const bytes = Buffer.from(getFilledUint8(32));
//     expect(bytes).to.be.instanceof(Buffer);
//     const res = testBufferAsBuffer(bytes);
//     expect(res).to.be.instanceof(Buffer);
//     expectHex(bytes, res);
//   });
//   it("should take buffer and return as hex string", () => {
//     const bytes = Buffer.from(getFilledUint8(32));
//     expect(bytes).to.be.instanceof(Buffer);
//     const res = testBufferAsString(bytes);
//     expect(res).to.be.string;
//     expectHex(bytes, res);
//   });
//   it("should take a Uint8Array and return as Uint8Array", () => {
//     const bytes = getFilledUint8(32);
//     expect(bytes).to.be.instanceof(Uint8Array);
//     const res = testTypedArrayAsTypedArray(bytes);
//     expect(res).to.be.instanceof(Uint8Array);
//     expectHex(bytes, res);
//   });
//   it("should take Uint8Array and return as hex string", () => {
//     const bytes = getFilledUint8(32);
//     expect(bytes).to.be.instanceof(Uint8Array);
//     const res = testTypedArrayAsString(bytes);
//     expect(res).to.be.string;
//     expectHex(bytes, res);
//   });
//   it("should take a hex string return a buffer", () => {
//     const bytes = Buffer.from(getFilledUint8(32, "imalittleteapot")).toString("hex");
//     expect(bytes).to.be.string;
//     const res = testStringAsBuffer(bytes);
//     expect(res).to.be.instanceof(Buffer);
//     expectHex(bytes, res);
//   });
// });
