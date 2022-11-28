import {findBindingByName} from "../scripts/paths";
import * as types from "./bindings.types";

interface Bindings {
  SecretKey: types.SecretKeyConstructor;
  PublicKey: types.PublicKeyConstructor;
  Signature: types.SignatureConstructor;
  aggregatePublicKeys: (pks: (string | Uint8Array)[]) => Promise<types.PublicKey>;
  DST: string;
  SECRET_KEY_LENGTH: number;
  PUBLIC_KEY_LENGTH_UNCOMPRESSED: number;
  PUBLIC_KEY_LENGTH_COMPRESSED: number;
  SIGNATURE_LENGTH_UNCOMPRESSED: number;
  SIGNATURE_LENGTH_COMPRESSED: number;
}

// eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
const bindings: Bindings = require(findBindingByName("blst-ts.node"));
types.PublicKey;
export = bindings;
