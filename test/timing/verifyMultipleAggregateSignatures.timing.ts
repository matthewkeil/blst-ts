import * as swigBindings from "../../src/swig/lib";
import {functions} from "../../src/lib/bindings";
import {makeNapiTestSets, NapiTestSet} from "../utils";

interface TestTime {
  startTime: number;
  endTime: number;
}
function makeSwigSet({msg, publicKey, signature}: NapiTestSet): swigBindings.SignatureSet {
  return {
    msg: Buffer.from(msg),
    pk: swigBindings.PublicKey.fromBytes(publicKey.serialize()),
    sig: swigBindings.Signature.fromBytes(signature.serialize()),
  };
}
async function swigSingleTest(sets: swigBindings.SignatureSet[]): Promise<TestTime> {
  const startTime = Date.now();
  const res = swigBindings.verifyMultipleAggregateSignatures(sets);
  if (!res) {
    throw new Error("invalid verification");
  }
  const endTime = Date.now();
  return {
    startTime,
    endTime,
  };
}
async function swigLoopTest(sets: swigBindings.SignatureSet[], loopCount: number): Promise<TestTime> {
  const startTime = Date.now();
  for (let i = 0; i < loopCount; i++) {
    const res = swigBindings.verifyMultipleAggregateSignatures(sets);
    if (!res) {
      throw new Error("invalid verification");
    }
  }
  const endTime = Date.now();
  return {
    startTime,
    endTime,
  };
}
async function napiSingleTest(sets: NapiTestSet[]): Promise<TestTime> {
  const startTime = Date.now();
  const res = await functions.verifyMultipleAggregateSignaturesAsync(sets);
  if (!res) {
    throw new Error("invalid verification");
  }
  const endTime = Date.now();
  return {
    startTime,
    endTime,
  };
}
async function napiLoopTest(sets: NapiTestSet[], loopCount: number): Promise<TestTime> {
  const startTime = Date.now();
  const responses: Promise<boolean>[] = [];
  for (let i = 0; i < loopCount; i++) {
    responses.push(
      functions.verifyMultipleAggregateSignaturesAsync(sets).then((res) => {
        if (!res) {
          throw new Error("invalid verification");
        }
        return res;
      })
    );
  }
  await Promise.all(responses);
  const endTime = Date.now();
  return {
    startTime,
    endTime,
  };
}

void (async function () {
  const napiSets = makeNapiTestSets(1000);
  const swigSets = napiSets.map(makeSwigSet);
  const swigSingle = await swigSingleTest(swigSets);
  const napiSingle = await napiSingleTest(napiSets);
  const loopCount = 1;
  const swigLoop = await swigLoopTest(swigSets, loopCount);
  const napiLoop = await napiLoopTest(napiSets, loopCount);
  console.log({
    swig: swigSingle.endTime - swigSingle.startTime,
    napi: napiSingle.endTime - napiSingle.startTime,
    swigLoop: swigLoop.endTime - swigLoop.startTime,
    napiLoop: napiLoop.endTime - napiLoop.startTime,
  });
})();
