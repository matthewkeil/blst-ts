/* eslint-disable @typescript-eslint/no-misused-new */
// interface NativeAddon {
//   new (name: string): NativeAddon;
//   greet(strName: string): string;
// }

import {P1_Affine} from "./bindings";

interface Bindings {
  // BlstTsAddon: NativeAddon;
  randomBytes: string;
  verifyMultipleAggregateSignatures: (pk: P1_Affine) => ArrayBuffer;
}

// eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
const bindings: Bindings = require("../build/Release/blst-ts-addon-native");

// class BlstTsAddon {
//   private _addonInstance: NativeAddon;

//   constructor(name: string) {
//     this._addonInstance = new bindings.BlstTsAddon(name);
//     console.log(bindings.randomBytes);
//     console.log(bindings.randomBytes.length);
//   }

//   greet(strName: string): string {
//     return this._addonInstance.greet(strName);
//   }
// }

export = bindings;
