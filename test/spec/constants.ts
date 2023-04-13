export const SPEC_TEST_ROOT_DIR = "tests";

export const SPEC_TEST_FORKS = ["phase0", "altair"] as const;
export type TestFork = typeof SPEC_TEST_FORKS[number];

export const PHASE0_BLS_TEST_TYPES = [
  "aggregate",
  "aggregate_verify",
  "fast_aggregate_verify",
  "sign",
  "verify",
] as const;
export type Phase0BlsTestType = typeof PHASE0_BLS_TEST_TYPES[number];

export const ALTAIR_BLS_TEST_TYPES = ["eth_aggregate_pubkeys", "eth_fast_aggregate_verify"] as const;
export type AltairBlsTestType = typeof ALTAIR_BLS_TEST_TYPES[number];

export type BlsTestType = Phase0BlsTestType | AltairBlsTestType;

export const G2_POINT_AT_INFINITY =
  "0xc00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
export const G1_POINT_AT_INFINITY =
  "0xc00000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000";
