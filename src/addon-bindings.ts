import {SignatureSet} from "./addon-bindings.types";

interface Bindings {
  verifyMultipleAggregateSignatures: (signatureSets: SignatureSet[]) => Promise<boolean>;
}

// eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
const bindings: Bindings = require("../build/Release/blst-ts-addon-native");

export = bindings;
