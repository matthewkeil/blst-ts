import fs from "fs";
import path from "path";

const ROOT_DIR = path.join(__dirname, "../..");
const BLST_NODE = "blst.node";
export const PREBUILD_DIR = path.join(ROOT_DIR, "prebuild");
export const PACKAGE_JSON_PATH = path.join(ROOT_DIR, "package.json");
export const BINDINGS_DIR = path.join(ROOT_DIR, "blst/bindings/node.js");

// Paths for blst_wrap.cpp
// Resolve path to absolute since it will be used from a different working dir
// when running blst_wrap.py
export const BLST_WRAP_CPP_PREBUILD = path.resolve(ROOT_DIR, "prebuild", "blst_wrap.cpp");

/**
 * Get binary name.
 * name: {platform}-{arch}-{v8 version}.node
 */
export function getBinaryName(): string {
  const platform = process.platform;
  const arch = process.arch;
  const nodeV8CppApiVersion = process.versions.modules;
  if (!process) throw new NotNodeJsError("global object");
  if (!platform) throw new NotNodeJsError("process.platform");
  if (!arch) throw new NotNodeJsError("process.arch");
  if (!process.versions.modules) throw new NotNodeJsError("process.versions.modules");

  return [platform, arch, nodeV8CppApiVersion, "binding.node"].join("-");
}

export function getBinaryPath(): string {
  return path.join(PREBUILD_DIR, getBinaryName());
}

export function mkdirBinary(): void {
  if (!fs.existsSync(PREBUILD_DIR)) {
    fs.mkdirSync(PREBUILD_DIR);
  }
}

export function ensureDirFromFilepath(filepath: string): void {
  const dirpath = path.dirname(filepath);
  if (!fs.existsSync(dirpath)) {
    fs.mkdirSync(dirpath, {recursive: true});
  }
}

function getFilePaths(root: string, name: string): string[] {
  return [
    // In dirpath
    [root, name],
    // node-gyp's linked version in the "build" dir
    [root, "build", name],
    // node-waf and gyp_addon (a.k.a node-gyp)
    [root, "build", "Debug", name],
    [root, "build", "Release", name],
    // Debug files, for development (legacy behavior, remove for node v0.9)
    [root, "out", "Debug", name],
    [root, "Debug", name],
    // Release files, but manually compiled (legacy behavior, remove for node v0.9)
    [root, "out", "Release", name],
    [root, "Release", name],
    // Legacy from node-waf, node <= 0.4.x
    [root, "build", "default", name],
    // Production "Release" buildtype binary (meh...)
    [root, "compiled", "version", "platform", "arch", name],
    // node-qbs builds
    [root, "addon-build", "release", "install-root", name],
    [root, "addon-build", "debug", "install-root", name],
    [root, "addon-build", "default", "install-root", name],
    // node-pre-gyp path ./lib/binding/{node_abi}-{platform}-{arch}
    [root, "lib", "binding", "nodePreGyp", name],
  ].map((pathParts) => path.join(...pathParts));
}

function searchPaths(filepaths: string[]): string {
  for (const filepath of filepaths) {
    if (fs.existsSync(filepath)) {
      console.log(filepath);
      return filepath;
    }
  }
  throw Error(`Could not find bindings file. Tried:\n${filepaths.join("\n")}`);
}

/**
 * The output of node-gyp is not at a predictable path but various
 * depending on the OS.
 * Paths based on https://github.com/TooTallNate/node-bindings/blob/c8033dcfc04c34397384e23f7399a30e6c13830d/bindings.js#L36
 */
export function findBindingByName(bindingName: string): string {
  return searchPaths(getFilePaths(ROOT_DIR, bindingName));
}

/**
 * The output of node-gyp is not at a predictable path but various
 * depending on the OS.
 * Paths based on https://github.com/TooTallNate/node-bindings/blob/c8033dcfc04c34397384e23f7399a30e6c13830d/bindings.js#L36
 */
export function findBindingsFile(dirpath: string): string {
  return searchPaths(getFilePaths(dirpath, BLST_NODE));
}

export class NotNodeJsError extends Error {
  constructor(missingItem: string) {
    super(`BLST bindings loader should only run in a NodeJS context: ${missingItem}`);
  }
}
