import {itBench} from "@dapplion/benchmark";
import * as swigBindings from "../../src/swig/lib";
import napiBindings from "../../src/lib/bindings";
import {linspace} from "../utils";
import {
  getSwigSet,
  getNapiSet,
  // getSwigPublicKey,
  // getNapiPublicKey
} from "./perf.helpers";

// describe("aggregatePublicKeys", () => {
//   for (const count of [3, 8, 32, 100]) {
//     describe(`${count} sets`, () => {
//       itBench({
//         id: `aggregatePubkeys ${count} - swig implementation`,
//         beforeEach: () => linspace(0, count - 1).map((i) => getSwigPublicKey(i)),
//         fn: (sets) => {
//           const isValid = swigBindings.aggregatePubkeys(sets);
//           if (!isValid) throw Error("Invalid");
//         },
//       });

//       itBench({
//         id: `aggregatePublicKeys ${count} - napi native implementation`,
//         beforeEach: () => linspace(0, count - 1).map((i) => getNapiPublicKey(i)),
//         fn: (sets) => {
//           const isValid = napiBindings.functions.aggregatePublicKeys(sets);
//           if (!isValid) throw Error("Invalid");
//         },
//       });

//       itBench({
//         id: `aggregatePublicKeysAsync ${count} - napi promise implementation`,
//         beforeEach: () => linspace(0, count - 1).map((i) => getNapiPublicKey(i)),
//         fn: async (sets) => {
//           const isValid = await napiBindings.functions.aggregatePublicKeysAsync(sets);
//           if (!isValid) throw Error("Invalid");
//         },
//       });
//     });
//   }
// });
describe("verifyMultipleAggregateSignatures", () => {
  for (const count of [3, 8, 32, 100]) {
    describe(`${count} sets`, () => {
      itBench({
        id: `verifyMultipleSignatures ${count} - swig implementation`,
        beforeEach: () => linspace(0, count - 1).map((i) => getSwigSet(i)),
        fn: (sets) => {
          const isValid = swigBindings.verifyMultipleAggregateSignatures(sets);
          if (!isValid) throw Error("Invalid");
        },
      });

      itBench({
        id: `verifyMultipleAggregateSignatures ${count} - napi native implementation`,
        beforeEach: () => linspace(0, count - 1).map((i) => getNapiSet(i)),
        fn: (sets) => {
          const isValid = napiBindings.functions.verifyMultipleAggregateSignatures(sets);
          if (!isValid) throw Error("Invalid");
        },
      });

      itBench({
        id: `verifyMultipleAggregateSignaturesAsync ${count} - napi promise implementation`,
        beforeEach: () => linspace(0, count - 1).map((i) => getNapiSet(i)),
        fn: async (sets) => {
          const isValid = await napiBindings.functions.verifyMultipleAggregateSignaturesAsync(sets);
          if (!isValid) throw Error("Invalid");
        },
      });
    });
  }
});
