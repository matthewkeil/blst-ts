// import {itBench} from "@dapplion/benchmark";
// import * as swigBindings from "../../deprecated/src/swig/lib";
// import * as napiBindings from "../../src";
// import {linspace} from "../utils";
// import {
//   getSwigSet,
//   getNapiSet,
//   // getSwigPublicKey,
//   // getNapiPublicKey
// } from "./perf.helpers";

// // describe("aggregatePublicKeys", () => {
// //   for (const count of [3, 8, 32, 100]) {
// //     describe(`${count} sets`, () => {
// //       itBench({
// //         id: `aggregatePubkeys ${count} - swig implementation`,
// //         beforeEach: () => linspace(0, count - 1).map((i) => getSwigPublicKey(i)),
// //         fn: (sets) => {
// //           const isValid = swigBindings.aggregatePubkeys(sets);
// //           if (!isValid) throw Error("Invalid");
// //         },
// //       });

// //       itBench({
// //         id: `aggregatePublicKeys ${count} - napi native implementation`,
// //         beforeEach: () => linspace(0, count - 1).map((i) => getNapiPublicKey(i)),
// //         fn: (sets) => {
// //           const isValid = napiBindings.functions.aggregatePublicKeys(sets);
// //           if (!isValid) throw Error("Invalid");
// //         },
// //       });

// //       itBench({
// //         id: `aggregatePublicKeysAsync ${count} - napi promise implementation`,
// //         beforeEach: () => linspace(0, count - 1).map((i) => getNapiPublicKey(i)),
// //         fn: async (sets) => {
// //           const isValid = await napiBindings.functions.aggregatePublicKeysAsync(sets);
// //           if (!isValid) throw Error("Invalid");
// //         },
// //       });
// //     });
// //   }
// // });
// describe("aggregateVerify", () => {
//   for (const count of [3, 8, 32, 100]) {
//     describe(`${count} sets`, () => {
//       itBench({
//         id: `aggregateVerify ${count} - swig implementation`,
//         beforeEach: () => linspace(0, count - 1).map((i) => getSwigSet(i)),
//         fn: (sets) => {
//           const msgs = sets.map(({msg}) => msg);
//           const pks = sets.map(({pk}) => pk);
//           const sig = swigBindings.aggregateSignatures(sets.map(({sig}) => sig));
//           const isValid = swigBindings.aggregateVerify(msgs, pks, sig);
//           if (!isValid) throw Error("Invalid");
//         },
//       });
//       // itBench({
//       //   id: `aggregateVerify ${count} - napi native implementation`,
//       //   beforeEach: () => linspace(0, count - 1).map((i) => getNapiSet(i)),
//       //   fn: (sets) => {
//       //     const isValid = napiBindings.functions.verifyMultipleAggregateSignatures(sets);
//       //     if (!isValid) throw Error("Invalid");
//       //   },
//       // });
//       // itBench({
//       //   id: `aggregateVerifyAsync ${count} - napi promise implementation`,
//       //   beforeEach: () => linspace(0, count - 1).map((i) => getNapiSet(i)),
//       //   fn: async (sets) => {
//       //     const isValid = await napiBindings.functions.verifyMultipleAggregateSignaturesAsync(sets);
//       //     if (!isValid) throw Error("Invalid");
//       //   },
//       // });
//     });
//   }
// });
// describe.only("verifyMultipleAggregateSignatures", () => {
//   for (const count of [3, 8, 32, 100]) {
//     describe(`${count} sets`, () => {
//       itBench({
//         id: `verifyMultipleSignatures ${count} - swig implementation`,
//         beforeEach: () => linspace(0, count - 1).map((i) => getSwigSet(i)),
//         fn: (sets) => {
//           const isValid = swigBindings.verifyMultipleAggregateSignatures(sets);
//           if (!isValid) throw Error("Invalid");
//         },
//       });
//       itBench({
//         id: `verifyMultipleAggregateSignatures ${count} - napi native implementation`,
//         beforeEach: () => linspace(0, count - 1).map((i) => getNapiSet(i)),
//         fn: (sets) => {
//           const isValid = napiBindings.functions.verifyMultipleAggregateSignatures(sets);
//           if (!isValid) throw Error("Invalid");
//         },
//       });
//       itBench({
//         id: `verifyMultipleAggregateSignaturesAsync ${count} - napi promise implementation`,
//         beforeEach: () => linspace(0, count - 1).map((i) => getNapiSet(i)),
//         fn: async (sets) => {
//           const isValid = await napiBindings.functions.verifyMultipleAggregateSignatures(sets);
//           if (!isValid) throw Error("Invalid");
//         },
//       });
//     });
//   }
// });
