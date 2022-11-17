// import {blst} from "../../../src/bindings";
import {
  // PublicKey,
  SecretKey,
} from "../../../src/lib";
// import {fromHex} from "../../utils";

import {SignatureSet} from "../../../src/addon-bindings.types";
import {randomBytes, verifyMultipleAggregateSignatures} from "../../../src/addon-bindings";
console.log(randomBytes?.toString("hex"));
console.log(randomBytes?.length);

// const sample = {
//   keygen: "********************************", // Must be at least 32 bytes
//   p1: fromHex(
//     "0ae7e5822ba97ab07877ea318e747499da648b27302414f9d0b9bb7e3646d248be90c9fdaddfdb93485a6e9334f0109301f36856007e1bc875ab1b00dbf47f9ead16c5562d889d8b270002ade81e78d473204fcb51ede8659bce3d95c67903bc"
//   ),
//   p1Comp: fromHex("8ae7e5822ba97ab07877ea318e747499da648b27302414f9d0b9bb7e3646d248be90c9fdaddfdb93485a6e9334f01093"),
// };

void (async function () {
  const n = 1;
  const sets: SignatureSet[] = [];
  for (let i = 0; i < n; i++) {
    const msg = Uint8Array.from(Buffer.alloc(32, i));
    const sk = SecretKey.fromKeygen(Buffer.alloc(32, i));
    sets.push({msg, publicKey: sk.toPublicKey().serialize(), signature: sk.sign(msg).serialize()});
  }

  // const sets: any = [
  //   {
  //     msg:
  //       // Buffer.alloc(32, "a")
  //       Uint8Array.from(Buffer.alloc(32, "a")),
  //   },
  // ];
  const result = await verifyMultipleAggregateSignatures(sets as any);
  console.log(result);
})();
