import {findBindingByName} from "../scripts/paths";

declare class SecretKey {
  static keygen(ikm?: Uint8Array): SecretKey;
  toBytes(): Uint8Array;
}

interface SecretKeyConstructor {
  new (): SecretKey;
  keygen(ikm?: Uint8Array): SecretKey;
  fromBytes(skBytes: Uint8Array): SecretKey;
}

interface Bindings {
  SecretKey: SecretKeyConstructor;
  test(): undefined;
}

// eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
const bindings: Bindings = require(findBindingByName("blst-ts.node"));

console.log(Object.keys(bindings));
const sk1 = bindings.SecretKey.keygen();

const rebuilt = bindings.SecretKey.fromBytes(sk1.toBytes());

console.log({
  key1: Buffer.from(sk1.toBytes()).toString("hex"),
  key2: Buffer.from(rebuilt.toBytes()).toString("hex"),
});
