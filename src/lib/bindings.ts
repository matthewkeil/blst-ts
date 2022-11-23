import {findBindingByName} from "../scripts/paths";
import {PublicKeyConstructor, SecretKeyConstructor, SignatureConstructor} from "./bindings.types";

interface Bindings {
  SecretKey: SecretKeyConstructor;
  PublicKey: PublicKeyConstructor;
  Signature: SignatureConstructor;
  DST: string;
  SECRET_KEY_LENGTH: number;
  PUBLIC_KEY_LENGTH_UNCOMPRESSED: number;
  PUBLIC_KEY_LENGTH_COMPRESSED: number;
  SIGNATURE_LENGTH_UNCOMPRESSED: number;
  SIGNATURE_LENGTH_COMPRESSED: number;
}

// eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
const bindings: Bindings = require(findBindingByName("blst-ts.node"));

export = bindings;

