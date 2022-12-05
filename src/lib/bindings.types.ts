import {blst, BLST_ERROR} from "./blst.hpp";
export {BLST_ERROR};

export const SECRET_KEY_LENGTH = 32;
export const PUBLIC_KEY_LENGTH_COMPRESSED = 48;
export const PUBLIC_KEY_LENGTH_UNCOMPRESSED = 48 * 2;
export const SIGNATURE_LENGTH_COMPRESSED = 96;
export const SIGNATURE_LENGTH_UNCOMPRESSED = 96 * 2;

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
export declare class SecretKey {
  constructor();
  static keygen(ikm?: NapiBuffer): SecretKey;
  // static keygenAsync(ikm?: Uint8Array| Buffer): Promise<SecretKey>;
  static fromBytes(skBytes: NapiBuffer): SecretKey;
  // static fromBytesAsync(skBytes: Uint8Array| Buffer): Promise<SecretKey>;
  getPublicKey(): PublicKey;
  // toPublicKeyAsync(): Promise<PublicKey>;
  sign(msg: NapiBuffer): Signature;
  // signAsync(msg: Uint8Array| Buffer): Promise<Signature>;
  toBytes(): NapiBuffer;
  // toBytesAsync(): Promise<Uint8Array>;
  serialize(): NapiBuffer;
  // serializeAsync(): Promise<Uint8Array>;
}
export interface SecretKeyConstructor {
  new (): SecretKey;
  keygen(ikm?: NapiBuffer): SecretKey;
  // keygenAsync(ikm?: Uint8Array| Buffer): Promise<SecretKey>;
  fromBytes(skBytes: NapiBuffer): SecretKey;
  // fromBytesAsync(skBytes: Uint8Array| Buffer): Promise<SecretKey>;
}

// export type BlstP1 = InstanceType<typeof blst.P1>;
// export type BlstP1Affine = InstanceType<typeof blst.P1_Affine>;
export declare class PublicKey {
  constructor(sk?: SecretKey);
  static fromBytes(bytes: NapiBuffer, coordType?: CoordType): PublicKey;
  // static fromBytesAsync(bytes: NapiBuffer, coordType?: CoordType): Promise<PublicKey>;
  toBytes(): Uint8Array;
  compress(): Uint8Array;
  serialize(): Uint8Array;
  keyValidate(): void;
  // keyValidateAsync(): Promise<void>;
}
export interface PublicKeyConstructor {
  new (sk?: SecretKey): PublicKey;
  fromBytes(bytes: NapiBuffer, coordType?: CoordType): PublicKey;
  // fromBytesAsync(): Promise<PublicKey>;
}

type BlstSignature = InstanceType<typeof blst.P2>;
type BlstSignatureAffine = InstanceType<typeof blst.P2_Affine>;
export declare class Signature {
  private constructor();
  static fromBytes(bytes: NapiBuffer): Signature;
  // static fromBytesAsync(): Promise<Signature>;
  get jacobian(): BlstSignature;
  get affine(): BlstSignatureAffine;
  toBytes(): Uint8Array;
  compress(): Uint8Array;
  serialize(): Uint8Array;
  sigValidate(): void;
  // sigValidateAsync(): Promise<void>;
}
export interface SignatureConstructor {
  // new (): Signature;
  fromBytes(bytes: NapiBuffer): Signature;
  // fromBytesAsync(): Promise<Signature>;
}

export interface SignatureSet {
  msg: Uint8Array;
  publicKey: Uint8Array;
  signature: Uint8Array;
}

export interface TestFunctions {
  testBufferAsBuffer(buff: Buffer): Buffer;
  testBufferAsString(buff: Buffer): string;
  testStringAsBuffer(str: string): Buffer;
  testTypedArrayAsTypedArray(buff: Uint8Array): Uint8Array;
  testTypedArrayAsString(buff: Uint8Array): string;
}

export interface BlstTsFunctions {
  tests: TestFunctions;
  aggregatePublicKeys(keys: ByteArray[]): Promise<PublicKey>;
}
