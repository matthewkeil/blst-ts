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

export type BlstBuffer = Uint8Array | Buffer;
export type PublicKeyArg = BlstBuffer | PublicKey;
export type SignatureArg = BlstBuffer | Signature;
export interface SignatureSet {
  msg: BlstBuffer;
  publicKey: PublicKeyArg;
  signature: SignatureArg;
}
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
  static fromKeygen(ikm?: BlstBuffer): Promise<SecretKey>;
  static fromKeygenSync(ikm?: BlstBuffer): SecretKey;
  static deserialize(skBytes: BlstBuffer): SecretKey;
  serialize(compress?: boolean): Buffer;
  toPublicKey(): Promise<PublicKey>;
  toPublicKeySync(): PublicKey;
  sign(msg: BlstBuffer): Promise<Signature>;
  signSync(msg: BlstBuffer): Signature;
}
export class PublicKey implements Serializable {
  constructor(sk: BlstBuffer | SecretKey);
  static deserialize(skBytes: BlstBuffer, coordType?: CoordType): PublicKey;
  serialize(compress?: boolean): Buffer;
  keyValidate(): Promise<void>;
  keyValidateSync(): void;
}
export class Signature implements Serializable {
  private constructor();
  static deserialize(skBytes: BlstBuffer, coordType?: CoordType): Signature;
  serialize(compress?: boolean): Buffer;
  sigValidate(): Promise<void>;
  sigValidateSync(): void;
}
export const functions: {
  aggregatePublicKeys(keys: PublicKeyArg[]): Promise<PublicKey>;
  aggregatePublicKeysSync(keys: PublicKeyArg[]): PublicKey;
  aggregateSignatures(signatures: SignatureArg[]): Promise<Signature>;
  aggregateSignaturesSync(signatures: SignatureArg[]): Signature;
  verify(msgs: BlstBuffer, publicKeys: PublicKeyArg, signature: SignatureArg): Promise<boolean>;
  verifySync(msgs: BlstBuffer, publicKeys: PublicKeyArg, signature: SignatureArg): boolean;
  aggregateVerify(msgs: BlstBuffer[], publicKeys: PublicKeyArg[], signature: SignatureArg): Promise<boolean>;
  aggregateVerifySync(msgs: BlstBuffer[], publicKeys: PublicKeyArg[], signature: SignatureArg): boolean;
  fastAggregateVerify(msgs: BlstBuffer, publicKeys: PublicKeyArg[], signature: SignatureArg): Promise<boolean>;
  fastAggregateVerifySync(msgs: BlstBuffer, publicKeys: PublicKeyArg[], signature: SignatureArg): boolean;
  verifyMultipleAggregateSignatures(sets: SignatureSet[]): Promise<boolean>;
  verifyMultipleAggregateSignaturesSync(sets: SignatureSet[]): boolean;
};
export const BLST_CONSTANTS: {
  DST: string;
  SECRET_KEY_LENGTH: number;
  PUBLIC_KEY_LENGTH_UNCOMPRESSED: number;
  PUBLIC_KEY_LENGTH_COMPRESSED: number;
  SIGNATURE_LENGTH_UNCOMPRESSED: number;
  SIGNATURE_LENGTH_COMPRESSED: number;
};
