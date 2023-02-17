/* eslint-disable @typescript-eslint/no-require-imports */
/* eslint-disable @typescript-eslint/no-var-requires */
// const {cmdStringExec} = require("../../tools/exec");
// import type {SecretKey} from "../../lib";

async function runLibrary() {
  // eslint-disable-next-line @typescript-eslint/no-var-requires, @typescript-eslint/no-require-imports
  const bindings = require("../../lib");
  // eslint-disable-next-line @typescript-eslint/no-unsafe-call
  const sk = await bindings.SecretKey.fromKeygen();
  const pk = sk.toPublicKey();
  await pk.keyValidate();
  const sig = await sk.sign(Buffer.from("fancy message"));
  console.log(sig);
}

// function startLeaks() {
//   const pid = process.pid;
//   return cmdStringExec("leaks " + pid);
// }

void (async function () {
  // const {child} = startLeaks();
  await runLibrary();
})();

