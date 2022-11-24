import {blst, BLST_ERROR} from "./blst.hpp";
export {BLST_ERROR};

export const SECRET_KEY_LENGTH = 32;
export const PUBLIC_KEY_LENGTH_COMPRESSED = 48;
export const PUBLIC_KEY_LENGTH_UNCOMPRESSED = 48 * 2;
export const SIGNATURE_LENGTH_COMPRESSED = 96;
export const SIGNATURE_LENGTH_UNCOMPRESSED = 96 * 2;

type NapiBuffer = Uint8Array | Buffer;
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
  static fromBytes(skBytes: NapiBuffer): SecretKey;
  // static fromBytesAsync(skBytes: Uint8Array| Buffer): Promise<SecretKey>;
  static keygen(ikm?: NapiBuffer): SecretKey;
  // static keygenAsync(ikm?: Uint8Array| Buffer): Promise<SecretKey>;
  getPublicKey(): PublicKey;
  // toPublicKeyAsync(): Promise<PublicKey>;
  sign(msg: NapiBuffer): Signature;
  // signAsync(msg: Uint8Array| Buffer): Promise<Signature>;
  toBytes(): NapiBuffer;
  // toBytesAsync(): Promise<Uint8Array>;
}
export interface SecretKeyConstructor {
  new (): SecretKey;
  keygen(ikm?: NapiBuffer): SecretKey;
  // keygenAsync(ikm?: Uint8Array| Buffer): Promise<SecretKey>;
  fromBytes(skBytes: NapiBuffer): SecretKey;
  // fromBytesAsync(skBytes: Uint8Array| Buffer): Promise<SecretKey>;
}

export type BlstP1 = InstanceType<typeof blst.P1>;
export type BlstP1Affine = InstanceType<typeof blst.P1_Affine>;
export declare class PublicKey {
  constructor(sk?: SecretKey);
  static fromBytes(bytes: NapiBuffer): PublicKey;
  static fromBytesAsync(bytes: NapiBuffer): Promise<PublicKey>;
  // static fromBytesAsyn(bytes: Uint8Array| Buffer): PublicKey;
  // get jacobian(): BlstP1;
  // get affine(): BlstP1Affine;
  toBytes(): Uint8Array;
  compress(): Uint8Array;
  serialize(): Uint8Array;
  keyValidate(): void;
  // keyValidateAsync(): Promise<void>;
}
export interface PublicKeyConstructor {
  new (sk?: SecretKey): PublicKey;
  fromBytes(): PublicKey;
  // fromBytesAsync(): Promise<PublicKey>;
}

type BlstSignature = InstanceType<typeof blst.P2>;
type BlstSignatureAffine = InstanceType<typeof blst.P2_Affine>;
export declare class Signature {
  private constructor();
  static fromBytes(): Signature;
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
  fromBytes(): Signature;
  // fromBytesAsync(): Promise<Signature>;
}

export interface SignatureSet {
  msg: Uint8Array;
  publicKey: Uint8Array;
  signature: Uint8Array;
}
