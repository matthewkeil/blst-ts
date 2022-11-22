import {findBindingByName} from "../scripts/paths";

declare class SecretKey {}

interface SecretKeyConstructor {
  new (): SecretKey;
}

interface Bindings {
  SecretKey: SecretKeyConstructor;
  test(): undefined;
}

// eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
const bindings: Bindings = require(findBindingByName("blst-ts.node"));

console.log(Object.keys(bindings));

console.log(new bindings.SecretKey());
