import chai, {expect} from "chai";
import deepEqualInAnyOrder from "deep-equal-in-any-order";
import fs from "fs";
import path from "path";
import jsYaml from "js-yaml";
import {SPEC_TEST_LOCATION, SPEC_TEST_TO_DOWNLOAD} from "./specTestVersioning";
import {BlsTestType, SPEC_TEST_FORKS, SPEC_TEST_ROOT_DIR, TestFork} from "./constants";
import {testFnByType, isBlstError} from "./testFunctions";

chai.use(deepEqualInAnyOrder);

// Example full path
// blst-ts/spec-tests/SPEC_TEST_ROOT_DIR/SPEC_TEST_TO_DOWNLOAD/SPEC_TEST_FORK/bls/TEST_TYPE/small/SPEC_NAME/data.yaml
// blst-ts/spec-tests/tests/general/altair/bls/eth_aggregate_pubkeys/small/eth_aggregate_pubkeys_empty_list/data.yaml

const testedTypes = [] as BlsTestType[];

const forkDirectory = path.join(SPEC_TEST_LOCATION, SPEC_TEST_ROOT_DIR, SPEC_TEST_TO_DOWNLOAD[0]); // blst-ts/spec-tests/tests/general
const testForks = fs.readdirSync(forkDirectory) as TestFork[];
before("Known testForks", () => {
  // eslint-disable-next-line @typescript-eslint/no-unsafe-call
  expect(testForks).to.deep.equalInAnyOrder(
    SPEC_TEST_FORKS,
    `Unknown/Missing testForks\nknown: ${SPEC_TEST_FORKS}\ntested: ${testForks}`
  );
});

for (const forkName of testForks) {
  const blsTestDir = path.join(forkDirectory, forkName, "bls"); // blst-ts/spec-tests/tests/general/altair/bls
  const blstTestTypes = fs.readdirSync(blsTestDir) as BlsTestType[];
  for (const testType of blstTestTypes) {
    testedTypes.push(testType);
    const testTypeDir = path.join(blsTestDir, testType); // blst-ts/spec-tests/tests/general/altair/bls/eth_aggregate_pubkeys
    const testFn = testFnByType[testType]; // eth_aggregate_pubkeys = () => { ... }
    const testName = path.join(forkName, testType); // altair/eth_aggregate_pubkeys
    describe(testName, () => {
      before("Known testFn", () => {
        if (!testFn) throw Error(`Unknown testFn ${testType}`);
      });

      const specGroups = fs.readdirSync(testTypeDir);
      for (const specGroup of specGroups) {
        // specGroup = "small"
        const specGroupDir = path.join(testTypeDir, specGroup); // blst-ts/spec-tests/tests/general/altair/bls/eth_aggregate_pubkeys/small

        for (const specName of fs.readdirSync(specGroupDir)) {
          // specName = "eth_aggregate_pubkeys_empty_list"
          const specDir = path.join(specGroupDir, specName); // blst-ts/spec-tests/tests/general/altair/bls/eth_aggregate_pubkeys/small/eth_aggregate_pubkeys_empty_list

          it(specName, () => {
            // Ensure there are no unknown files
            const files = fs.readdirSync(specDir);
            expect(files).to.deep.equal(["data.yaml"], `Unknown files in ${specDir}`);

            // Examples of parsed YAML
            // {
            //   input: [
            //     '0x91347bccf740d859038fcdcaf233eeceb2a436bcaaee9b2aa3bfb70efe29dfb2677562ccbea1c8e061fb9971b0753c240622fab78489ce96768259fc01360346da5b9f579e5da0d941e4c6ba18a0e64906082375394f337fa1af2b7127b0d121',
            //     '0x9674e2228034527f4c083206032b020310face156d4a4685e2fcaec2f6f3665aa635d90347b6ce124eb879266b1e801d185de36a0a289b85e9039662634f2eea1e02e670bc7ab849d006a70b2f93b84597558a05b879c8d445f387a5d5b653df',
            //     '0xae82747ddeefe4fd64cf9cedb9b04ae3e8a43420cd255e3c7cd06a8d88b7c7f8638543719981c5d16fa3527c468c25f0026704a6951bde891360c7e8d12ddee0559004ccdbe6046b55bae1b257ee97f7cdb955773d7cf29adf3ccbb9975e4eb9'
            //   ],
            //   output: '0x9712c3edd73a209c742b8250759db12549b3eaf43b5ca61376d9f30e2747dbcf842d8b2ac0901d2a093713e20284a7670fcf6954e9ab93de991bb9b313e664785a075fc285806fa5224c82bde146561b446ccfc706a64b8579513cfc4ff1d930'
            // }
            //
            // {
            //   input: ['0x000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000'],
            //   output: null
            // }
            //
            // {
            //   input: ...,
            //   output: false
            // }

            const testData = jsYaml.load(fs.readFileSync(path.join(specDir, "data.yaml"), "utf8")) as {
              input: unknown;
              output: unknown;
            };

            if (process.env.DEBUG) {
              // eslint-disable-next-line no-console
              console.log(testData);
            }

            try {
              expect(testFn(testData.input)).to.deep.equal(testData.output);
            } catch (e) {
              // spec test expect a boolean even for invalid inputs
              if (!isBlstError(e)) throw e;

              expect(false).to.deep.equal(Boolean(testData.output));
            }
          });
        }
      }
    });
  }
}

after("Known testTypes", () => {
  const knownTestTypes = Object.keys(testFnByType);
  // eslint-disable-next-line @typescript-eslint/no-unsafe-call
  expect(testedTypes).to.deep.equalInAnyOrder(
    knownTestTypes,
    `Unknown/Missing testTypes\nknown: ${knownTestTypes}\ntested: ${testedTypes}`
  );
});
