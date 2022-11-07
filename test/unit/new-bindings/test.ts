import {blst} from "../../../src/bindings";
import {PublicKey} from "../../../src/lib";
import {verifyMultipleAggregateSignatures} from "../../../src/addon-bindings";
import {fromHex} from "../../utils";

const sample = {
  keygen: "********************************", // Must be at least 32 bytes
  p1: fromHex(
    "0ae7e5822ba97ab07877ea318e747499da648b27302414f9d0b9bb7e3646d248be90c9fdaddfdb93485a6e9334f0109301f36856007e1bc875ab1b00dbf47f9ead16c5562d889d8b270002ade81e78d473204fcb51ede8659bce3d95c67903bc"
  ),
  p1Comp: fromHex("8ae7e5822ba97ab07877ea318e747499da648b27302414f9d0b9bb7e3646d248be90c9fdaddfdb93485a6e9334f01093"),
};

const sk = new blst.SecretKey();
sk.keygen(sample.keygen);
const p1 = new blst.P1(sk);
const p1Affine = new blst.P1_Affine(p1);
const pk = new PublicKey(p1Affine);

const arrayBuffer = verifyMultipleAggregateSignatures(pk.affine);

const uint8 = new Uint8Array(arrayBuffer);

const serialized = pk.affine.serialize();

for (let i = 0; i < arrayBuffer.byteLength; i++) {
  const oldBindings = serialized[i];
  const newBindings = uint8[i];
  if (oldBindings !== newBindings) {
    console.log({oldBindings, newBindings});
  }
}
