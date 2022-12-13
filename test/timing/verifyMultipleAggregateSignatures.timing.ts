import * as swigBindings from "../../src/swig/lib";
import {functions} from "../../src/lib/bindings";
import {SignatureSet} from "../../src/lib/bindings.types";

interface TestTime {
  startTime: number;
  endTime: number;
}

const msg = Buffer.from("i'm a little tea pot");

function makeSets(numSets: number): SignatureSet[] {
  const sets: SignatureSet[] = [];
  for (let i = 0; i < numSets; i++) {
    const sk = swigBindings.SecretKey.fromKeygen(Buffer.from("*".repeat(32)));
    const publicKey = sk.toPublicKey();
    const signature = sk.sign(msg);
    sets.push({msg, publicKey: publicKey.serialize(), signature: signature.serialize()});
  }
  return sets;
}
function makeSwigSet(set: SignatureSet): swigBindings.SignatureSet {
  return {
    msg: set.msg,
    pk: swigBindings.PublicKey.fromBytes(set.publicKey),
    sig: swigBindings.Signature.fromBytes(set.signature),
  };
}
async function swigSingleTest(sets: SignatureSet[]): Promise<TestTime> {
  const startTime = Date.now();
  const res = swigBindings.verifyMultipleAggregateSignatures(sets.map(makeSwigSet));
  if (!res) {
    throw new Error("invalid verification");
  }
  const endTime = Date.now();
  return {
    startTime,
    endTime,
  };
}
async function swigLoopTest(sets: SignatureSet[], loopCount: number): Promise<TestTime> {
  const startTime = Date.now();
  for (let i = 0; i < loopCount; i++) {
    const res = swigBindings.verifyMultipleAggregateSignatures(sets.map(makeSwigSet));
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
async function napiSingleTest(sets: SignatureSet[]): Promise<TestTime> {
  const startTime = Date.now();
  const res = await functions.verifyMultipleAggregateSignatures(sets);
  if (!res) {
    throw new Error("invalid verification");
  }
  const endTime = Date.now();
  return {
    startTime,
    endTime,
  };
}
async function napiLoopTest(sets: SignatureSet[], loopCount: number): Promise<TestTime> {
  const startTime = Date.now();
  const responses: Promise<boolean>[] = [];
  for (let i = 0; i < loopCount; i++) {
    responses.push(
      functions.verifyMultipleAggregateSignatures(sets).then((res) => {
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
  const sets = makeSets(1000);
  const swigSingle = await swigSingleTest(sets);
  const napiSingle = await napiSingleTest(sets);
  const loopCount = 10;
  const swigLoop = await swigLoopTest(sets, loopCount);
  const napiLoop = await napiLoopTest(sets, loopCount);
  console.log({
    swig: swigSingle.endTime - swigSingle.startTime,
    napi: napiSingle.endTime - napiSingle.startTime,
    swigLoop: swigLoop.endTime - swigLoop.startTime,
    napiLoop: napiLoop.endTime - napiLoop.startTime,
  });
})();
