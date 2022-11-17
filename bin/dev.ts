/* eslint-disable no-console */
import {resolve} from "path";
import {exec, watchFolder} from "../lib";

const SRC_FOLDER = resolve(__dirname, "..", "src");
const ADDON_FOLDER = resolve(SRC_FOLDER, "addon");
const DEBOUNCE_TIME = 100;

let debounceCompile: undefined | NodeJS.Timeout;
async function compile(): Promise<void> {
  if (!debounceCompile) {
    debounceCompile = setTimeout(() => {
      if (debounceCompile) {
        clearTimeout(debounceCompile);
      }
      debounceCompile = undefined;
    }, DEBOUNCE_TIME);
    exec("npm run build:addon").then(console.log).catch(console.error);
  }
}

async function buildWatchAddon(): Promise<void> {
  // run node-gyp compile
  void compile();
  // watch src folder and rerun compile on file changes
  return watchFolder({
    path: ADDON_FOLDER,
    onAdd: () => compile(),
    onChange: () => compile(),
    onUnlink: () => compile(),
    onUnlinkDir: () => compile(),
    onAddDir: () => {
      /* no-op */
    },
  });
}

if (require.main === module) {
  void buildWatchAddon();
}
