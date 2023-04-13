import * as blst from "../../src/lib";
import {fromHex, toHex} from "../utils";
import {G1_POINT_AT_INFINITY, G2_POINT_AT_INFINITY} from "./constants";

/**
 * ```
 * input: List[BLS Signature] -- list of input BLS signatures
 * output: BLS Signature -- expected output, single BLS signature or empty.
 * ```
 */
function aggregate(input: string[]): string | null {
  const agg = blst.aggregateSignatures(input.map((hex) => blst.Signature.fromBytes(fromHex(hex))));
  return toHex(agg.toBytes());
}

/**
 * ```
 * input:
 *   pubkeys: List[BLS Pubkey] -- the pubkeys
 *   messages: List[bytes32] -- the messages
 *   signature: BLS Signature -- the signature to verify against pubkeys and messages
 * output: bool  --  true (VALID) or false (INVALID)
 * ```
 */
function aggregate_verify(input: {pubkeys: string[]; messages: string[]; signature: string}): boolean {
  const {pubkeys, messages, signature} = input;
  return blst.aggregateVerify(
    messages.map(fromHex),
    pubkeys.map((hex) => blst.PublicKey.fromBytes(fromHex(hex))),
    blst.Signature.fromBytes(fromHex(signature))
  );
}

/**
 * ```
 * input: List[BLS Signature] -- list of input BLS signatures
 * output: BLS Signature -- expected output, single BLS signature or empty.
 * ```
 */
function eth_aggregate_pubkeys(input: string[]): string | null {
  // Don't add this checks in the source as beacon nodes check the pubkeys for inf when onboarding
  for (const pk of input) {
    if (pk === G1_POINT_AT_INFINITY) return null;
  }

  const agg = blst.aggregatePubkeys(input.map((hex) => blst.PublicKey.fromBytes(fromHex(hex))));
  return toHex(agg.toBytes());
}

/**
 * ```
 * input:
 *   pubkeys: List[BLS Pubkey] -- list of input BLS pubkeys
 *   message: bytes32 -- the message
 *   signature: BLS Signature -- the signature to verify against pubkeys and message
 * output: bool  --  true (VALID) or false (INVALID)
 * ```
 */
function eth_fast_aggregate_verify(input: {pubkeys: string[]; message: string; signature: string}): boolean {
  const {pubkeys, message, signature} = input;

  if (pubkeys.length === 0 && signature === G2_POINT_AT_INFINITY) {
    return true;
  }

  // Don't add this checks in the source as beacon nodes check the pubkeys for inf when onboarding
  for (const pk of pubkeys) {
    if (pk === G1_POINT_AT_INFINITY) return false;
  }

  return blst.fastAggregateVerify(
    fromHex(message),
    pubkeys.map((hex) => blst.PublicKey.fromBytes(fromHex(hex))),
    blst.Signature.fromBytes(fromHex(signature))
  );
}

/**
 * ```
 * input:
 *   pubkeys: List[BLS Pubkey] -- list of input BLS pubkeys
 *   message: bytes32 -- the message
 *   signature: BLS Signature -- the signature to verify against pubkeys and message
 * output: bool  --  true (VALID) or false (INVALID)
 * ```
 */
function fast_aggregate_verify(input: {pubkeys: string[]; message: string; signature: string}): boolean | null {
  const {pubkeys, message, signature} = input;

  // Don't add this checks in the source as beacon nodes check the pubkeys for inf when onboarding
  for (const pk of pubkeys) {
    if (pk === G1_POINT_AT_INFINITY) return false;
  }

  return blst.fastAggregateVerify(
    fromHex(message),
    pubkeys.map((hex) => blst.PublicKey.fromBytes(fromHex(hex))),
    blst.Signature.fromBytes(fromHex(signature))
  );
}

/**
 * input:
 *   privkey: bytes32 -- the private key used for signing
 *   message: bytes32 -- input message to sign (a hash)
 * output: BLS Signature -- expected output, single BLS signature or empty.
 */
function sign(input: {privkey: string; message: string}): string | null {
  const {privkey, message} = input;
  const sk = blst.SecretKey.fromBytes(fromHex(privkey));
  const signature = sk.sign(fromHex(message));
  return toHex(signature.toBytes());
}

/**
 * input:
 *   pubkey: bytes48 -- the pubkey
 *   message: bytes32 -- the message
 *   signature: bytes96 -- the signature to verify against pubkey and message
 * output: bool  -- VALID or INVALID
 */
function verify(input: {pubkey: string; message: string; signature: string}): boolean {
  const {pubkey, message, signature} = input;
  return blst.verify(
    fromHex(message),
    blst.PublicKey.fromBytes(fromHex(pubkey)),
    blst.Signature.fromBytes(fromHex(signature))
  );
}

export function isBlstError(e: unknown): boolean {
  return (e as Error).message.includes("BLST_ERROR") || e instanceof blst.ErrorBLST;
}

export const testFnByType: Record<string, (data: any) => any> = {
  aggregate,
  aggregate_verify,
  eth_aggregate_pubkeys,
  eth_fast_aggregate_verify,
  fast_aggregate_verify,
  sign,
  verify,
};
