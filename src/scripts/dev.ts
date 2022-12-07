/* eslint-disable no-console */
import {resolve} from "path";
import {watchWithCallback} from "./watch";
import {cmdStringExec} from "./exec";

const ROOT_FOLDER = resolve(__dirname, "..", "..");
const SRC_FOLDER = resolve(ROOT_FOLDER, "src");
const BINDINGS_FOLDER = resolve(SRC_FOLDER, "bindings");

// const BUILD_FOLDER = resolve(ROOT_FOLDER, "build", "Release");

const DEBOUNCE_TIME = 200;

/**
 * Builds addon and then starts watch.
 * Watches src/addon folder and rerun compile on file changes
 */
void watchWithCallback({
  path: BINDINGS_FOLDER,
  debounceTime: DEBOUNCE_TIME,
  cb: () =>
    cmdStringExec("npm run build:addon", false)
      .then(console.log)
      .then(() => cmdStringExec("npm run test:bindings", false, {timeout: 20 * 1000}))
      .then(console.log)
      .catch(console.error),
});

// void watchWithCallback({
//   path: BUILD_FOLDER,
//   debounceTime: DEBOUNCE_TIME,
//   cb: () =>
//     cmdStringExec("npm run test:bindings", false)
//       .then(console.log)
//       .catch(() => void 0),
// });
