import fs from "fs";
import { exec } from "child_process";
import { testBindings } from "./testBindings";
import {
  bindingsDirSrc,
  bindingsSrc,
  ensureDirFromFilepath,
  prebuiltSwigSrc,
  prebuiltSwigTarget,
  blstWrapCppName,
} from "./paths";
import { downloadReleaseAsset } from "./downloadReleaseAsset";

export async function buildBindings(binaryPath: string) {
  // Make sure SWIG generated bindings are available or download from release assets
  if (fs.existsSync(prebuiltSwigSrc)) {
    fs.copyFileSync(prebuiltSwigSrc, prebuiltSwigTarget);
  } else {
    try {
      await downloadReleaseAsset(blstWrapCppName, prebuiltSwigTarget);
    } catch (e) {
      // TODO: Make sure SWIG is available or throw
      console.error(
        `Error downloading prebuilt ${blstWrapCppName}. Trying to built it from source with SWIG\n${e.message}`
      );
    }
  }

  // Use BLST run.me script to build libblst.a + blst.node
  await new Promise((resolve, reject): void => {
    const proc = exec(
      "./run.me",
      {
        timeout: 3 * 60 * 1000, // ms
        maxBuffer: 10e6, // bytes
        cwd: bindingsDirSrc,
      },
      (err, stdout, stderr) => {
        if (err) reject(err);
        else resolve(stdout.trim() || stderr);
      }
    );
    if (proc.stdout) proc.stdout.pipe(process.stdout);
    if (proc.stderr) proc.stderr.pipe(process.stderr);
  });

  // Copy built .node file to expected path
  ensureDirFromFilepath(binaryPath);
  fs.copyFileSync(bindingsSrc, binaryPath);

  // Make sure downloaded bindings work
  await testBindings(binaryPath);
}
