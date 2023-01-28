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

export interface Serializable {
  serialize(): Uint8Array;
}

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
export class SecretKey implements Serializable {
  private constructor();
  static fromKeygen(ikm?: NapiBuffer): Promise<SecretKey>;
  static fromKeygenSync(ikm?: NapiBuffer): SecretKey;
  static deserialize(skBytes: NapiBuffer): SecretKey;
  serialize(compress?: boolean): Buffer;
  toPublicKey(): Promise<PublicKey>;
  toPublicKeySync(): PublicKey;
  sign(msg: NapiBuffer): Promise<Signature>;
  signSync(msg: NapiBuffer): Signature;
}
export class PublicKey implements Serializable {
  constructor(sk: NapiBuffer | SecretKey);
  static deserialize(skBytes: NapiBuffer, coordType?: CoordType): PublicKey;
  serialize(compress?: boolean): Buffer;
  keyValidate(): Promise<void>;
  keyValidateSync(): void;
}
export type PublicKeyArg = NapiBuffer | PublicKey;
export class Signature implements Serializable {
  private constructor();
  static deserialize(skBytes: NapiBuffer, coordType?: CoordType): Signature;
  serialize(compress?: boolean): Buffer;
  sigValidate(): Promise<void>;
  sigValidateSync(): void;
}
export type SignatureArg = NapiBuffer | Signature;
export interface SignatureSet {
  msg: NapiBuffer;
  publicKey: PublicKeyArg;
  signature: SignatureArg;
}
interface BlstTsFunctions {
  aggregatePublicKeys(keys: PublicKeyArg[]): Promise<PublicKey>;
  aggregatePublicKeysSync(keys: PublicKeyArg[]): PublicKey;
  aggregateSignatures(signatures: SignatureArg[]): Promise<Signature>;
  aggregateSignaturesSync(signatures: SignatureArg[]): Signature;
  verify(msgs: ByteArray, publicKeys: PublicKeyArg, signature: SignatureArg): Promise<boolean>;
  verifySync(msgs: ByteArray, publicKeys: PublicKeyArg, signature: SignatureArg): boolean;
  aggregateVerify(msgs: ByteArray[], publicKeys: PublicKeyArg[], signature: SignatureArg): Promise<boolean>;
  aggregateVerifySync(msgs: ByteArray[], publicKeys: PublicKeyArg[], signature: SignatureArg): boolean;
  fastAggregateVerify(msgs: ByteArray, publicKeys: PublicKeyArg[], signature: SignatureArg): Promise<boolean>;
  fastAggregateVerifySync(msgs: ByteArray, publicKeys: PublicKeyArg[], signature: SignatureArg): boolean;
  verifyMultipleAggregateSignatures(sets: SignatureSet[]): Promise<boolean>;
  verifyMultipleAggregateSignaturesSync(sets: SignatureSet[]): boolean;
}
export const functions: BlstTsFunctions;
export const DST: string;
export const SECRET_KEY_LENGTH: number;
export const PUBLIC_KEY_LENGTH_UNCOMPRESSED: number;
export const PUBLIC_KEY_LENGTH_COMPRESSED: number;
export const SIGNATURE_LENGTH_UNCOMPRESSED: number;
export const SIGNATURE_LENGTH_COMPRESSED: number;
