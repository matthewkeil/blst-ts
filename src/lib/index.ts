import {findBindingByName} from "../scripts/paths";

declare class SecretKey {
  static keygen(ikm?: Uint8Array): SecretKey;
}

interface SecretKeyConstructor {
  new (): SecretKey;
  keygen(ikm?: Uint8Array): SecretKey;
}

interface Bindings {
  SecretKey: SecretKeyConstructor;
  test(): undefined;
}

// eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
const bindings: Bindings = require(findBindingByName("blst-ts.node"));

console.log(Object.keys(bindings));

console.log(bindings.SecretKey.keygen());
