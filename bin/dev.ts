/* eslint-disable no-console */
import {resolve} from "path";
import {exec, watchFolder} from "../lib";

const ROOT_FOLDER = resolve(__dirname, "..");
const SRC_FOLDER = resolve(ROOT_FOLDER, "src");
const ADDON_FOLDER = resolve(SRC_FOLDER, "addon");

const BLST_FOLDER = resolve(ROOT_FOLDER, "blst");
const BLST_SRC_FOLDER = resolve(BLST_FOLDER, "src");

const DEBOUNCE_TIME = 100;

let debounceCompileAddon: undefined | NodeJS.Timeout;
async function compileAddon(): Promise<void> {
  if (!debounceCompileAddon) {
    debounceCompileAddon = setTimeout(() => {
      if (debounceCompileAddon) {
        clearTimeout(debounceCompileAddon);
      }
      debounceCompileAddon = undefined;
    }, DEBOUNCE_TIME);
    exec("npm run build:addon").then(console.log).catch(console.error);
  }
}

let debounceCompileBlst: undefined | NodeJS.Timeout;
async function compileBlst(): Promise<void> {
  if (!debounceCompileBlst) {
    debounceCompileBlst = setTimeout(() => {
      if (debounceCompileBlst) {
        clearTimeout(debounceCompileBlst);
      }
      debounceCompileBlst = undefined;
    }, DEBOUNCE_TIME);
    await exec(`cd ${BLST_FOLDER} && sh build.sh`).then(console.log).catch(console.error);
    await compileAddon();
  }
}

async function buildWatchAddon(): Promise<void> {
  // watch src folder and rerun compile on file changes
  return watchFolder({
    path: ADDON_FOLDER,
    onAdd: () => compileAddon(),
    onChange: () => compileAddon(),
    onUnlink: () => compileAddon(),
    onUnlinkDir: () => compileAddon(),
    onAddDir: () => {
      /* no-op */
    },
  });
}

async function buildWatchBlst(): Promise<void> {
  // watch src folder and rerun compile on file changes
  return watchFolder({
    path: BLST_SRC_FOLDER,
    onAdd: () => compileBlst(),
    onChange: () => compileBlst(),
    onUnlink: () => compileBlst(),
    onUnlinkDir: () => compileBlst(),
    onAddDir: () => {
      /* no-op */
    },
  });
}

void (async function () {
  await compileBlst();
  await compileAddon();
  void buildWatchBlst();
  void buildWatchAddon();
})();
