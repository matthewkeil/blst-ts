/**
 * Points are represented in two ways in BLST:
 * - affine coordinates (x,y)
 * - jacobian coordinates (x,y,z)
 *
 * The jacobian coordinates allow to aggregate points more efficiently,
 * so if P1 points are aggregated often (Eth2.0) you want to keep the point
 * cached in jacobian coordinates.
 */
export enum CoordType {
  affine,
  jacobian,
}

export type NapiBuffer = Uint8Array | Buffer;
export type ByteArray = NapiBuffer | string;
/*
 * Private constructor will randomly generate ikm when new'ing a key.
 * Use static methods SecretKey.fromBytes and SecretKey.keygen to
 * generate the SecretKey from your own material (ie serialized key
 * or ikm you have created).
 *
 * ```typescript
 * const keyInfo = "Some key info";
 * let key = new SecretKey(keyInfo);
 * // -- or --
 * const ikm = UintArray8.from(Buffer.from("your very own ikm"));
 * key: SecretKey = SecretKey.keygen(ikm, keyInfo);
 * // -- or --
 * key = SecretKey.fromBytes(key.serialize());
 * ```
 */
export class SecretKey {
  private constructor();
  static fromKeygen(ikm?: NapiBuffer): SecretKey;
  static fromKeygenSync(ikm?: NapiBuffer): SecretKey;
  static deserialize(skBytes: NapiBuffer): Promise<SecretKey>;
  static deserializeSync(skBytes: NapiBuffer): Promise<SecretKey>;
  toPublicKey(): PublicKey;
  toPublicKeySync(): Promise<PublicKey>;
  sign(msg: NapiBuffer): Signature;
  signSync(msg: NapiBuffer): Promise<Signature>;
  serialize(compress?: boolean): Buffer;
  serializeSync(compress?: boolean): Promise<Buffer>;
}
export class PublicKey {
  constructor(sk: NapiBuffer | SecretKey);
  static deserialize(skBytes: NapiBuffer, coordType?: CoordType): PublicKey;
  static deserializeSync(skBytes: NapiBuffer, coordType?: CoordType): PublicKey;
  keyValidate(): void;
  keyValidateSync(): Promise<void>;
  serialize(compress?: boolean): Buffer;
  serializeSync(compress?: boolean): Promise<Buffer>;
}
export type PublicKeyArg = NapiBuffer | PublicKey;
export class Signature {
  private constructor();
  static deserialize(skBytes: NapiBuffer, coordType?: CoordType): PublicKey;
  static deserializeSync(skBytes: NapiBuffer, coordType?: CoordType): PublicKey;
  sigValidate(): void;
  sigValidateSync(): Promise<void>;
  serialize(compress?: boolean): Buffer;
  serializeSync(compress?: boolean): Promise<Buffer>;
}
export type SignatureArg = NapiBuffer | Signature;
export interface SignatureSet {
  msg: NapiBuffer;
  publicKey: PublicKeyArg;
  signature: SignatureArg;
}
interface TestFunctions {
  testBufferAsBuffer(buff: Buffer): Buffer;
  testBufferAsString(buff: Buffer): string;
  testStringAsBuffer(str: string): Buffer;
  testTypedArrayAsTypedArray(buff: Uint8Array): Uint8Array;
  testTypedArrayAsString(buff: Uint8Array): string;
}
interface BlstTsFunctions {
  tests: TestFunctions;
  aggregatePublicKeys(keys: PublicKeyArg[]): PublicKey;
  aggregatePublicKeysAsync(keys: PublicKeyArg[]): Promise<PublicKey>;
  aggregateVerify(msgs: ByteArray[], publicKeys: PublicKeyArg[], signature: SignatureArg): boolean;
  aggregateVerifyAsync(msgs: ByteArray[], publicKeys: PublicKeyArg[], signature: SignatureArg): Promise<boolean>;
  verifyMultipleAggregateSignatures(sets: SignatureSet[]): boolean;
  verifyMultipleAggregateSignaturesAsync(sets: SignatureSet[]): Promise<boolean>;
}
export const functions: BlstTsFunctions;
export const DST: string;
export const SECRET_KEY_LENGTH: number;
export const PUBLIC_KEY_LENGTH_UNCOMPRESSED: number;
export const PUBLIC_KEY_LENGTH_COMPRESSED: number;
export const SIGNATURE_LENGTH_UNCOMPRESSED: number;
export const SIGNATURE_LENGTH_COMPRESSED: number;
