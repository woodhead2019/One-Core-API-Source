/*
 * Copyright 2004 Filip Navara
 * Based on public domain SHA code by Steve Reid <steve@edmweb.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA
 */

#include "main.h"

/* SHA1 algorithm
 *
 * Based on public domain SHA code by Steve Reid <steve@edmweb.com>
 */

typedef struct {
   ULONG Unknown[6];
   ULONG State[5];
   ULONG Count[2];
   UCHAR Buffer[64];
} SHA_CTX, *PSHA_CTX;

#define rol(value, bits) (((value) << (bits)) | ((value) >> (32 - (bits))))
/* FIXME: This definition of DWORD2BE is little endian specific! */
#define DWORD2BE(x) (((x) >> 24) & 0xff) | (((x) >> 8) & 0xff00) | (((x) << 8) & 0xff0000) | (((x) << 24) & 0xff000000);
/* FIXME: This definition of blk0 is little endian specific! */
#define blk0(i) (Block[i] = (rol(Block[i],24)&0xFF00FF00)|(rol(Block[i],8)&0x00FF00FF))
#define blk1(i) (Block[i&15] = rol(Block[(i+13)&15]^Block[(i+8)&15]^Block[(i+2)&15]^Block[i&15],1))
#define f1(x,y,z) (z^(x&(y^z)))
#define f2(x,y,z) (x^y^z)
#define f3(x,y,z) ((x&y)|(z&(x|y)))
#define f4(x,y,z) (x^y^z)
/* (R0+R1), R2, R3, R4 are the different operations used in SHA1 */
#define R0(v,w,x,y,z,i) z+=f1(w,x,y)+blk0(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R1(v,w,x,y,z,i) z+=f1(w,x,y)+blk1(i)+0x5A827999+rol(v,5);w=rol(w,30);
#define R2(v,w,x,y,z,i) z+=f2(w,x,y)+blk1(i)+0x6ED9EBA1+rol(v,5);w=rol(w,30);
#define R3(v,w,x,y,z,i) z+=f3(w,x,y)+blk1(i)+0x8F1BBCDC+rol(v,5);w=rol(w,30);
#define R4(v,w,x,y,z,i) z+=f4(w,x,y)+blk1(i)+0xCA62C1D6+rol(v,5);w=rol(w,30);

/* Hash a single 512-bit block. This is the core of the algorithm. */
static void SHA1Transform(ULONG State[5], UCHAR Buffer[64])
{
   ULONG a, b, c, d, e;
   ULONG *Block;

   Block = (ULONG*)Buffer;

   /* Copy Context->State[] to working variables */
   a = State[0];
   b = State[1];
   c = State[2];
   d = State[3];
   e = State[4];

   /* 4 rounds of 20 operations each. Loop unrolled. */
   R0(a,b,c,d,e, 0); R0(e,a,b,c,d, 1); R0(d,e,a,b,c, 2); R0(c,d,e,a,b, 3);
   R0(b,c,d,e,a, 4); R0(a,b,c,d,e, 5); R0(e,a,b,c,d, 6); R0(d,e,a,b,c, 7);
   R0(c,d,e,a,b, 8); R0(b,c,d,e,a, 9); R0(a,b,c,d,e,10); R0(e,a,b,c,d,11);
   R0(d,e,a,b,c,12); R0(c,d,e,a,b,13); R0(b,c,d,e,a,14); R0(a,b,c,d,e,15);
   R1(e,a,b,c,d,16); R1(d,e,a,b,c,17); R1(c,d,e,a,b,18); R1(b,c,d,e,a,19);
   R2(a,b,c,d,e,20); R2(e,a,b,c,d,21); R2(d,e,a,b,c,22); R2(c,d,e,a,b,23);
   R2(b,c,d,e,a,24); R2(a,b,c,d,e,25); R2(e,a,b,c,d,26); R2(d,e,a,b,c,27);
   R2(c,d,e,a,b,28); R2(b,c,d,e,a,29); R2(a,b,c,d,e,30); R2(e,a,b,c,d,31);
   R2(d,e,a,b,c,32); R2(c,d,e,a,b,33); R2(b,c,d,e,a,34); R2(a,b,c,d,e,35);
   R2(e,a,b,c,d,36); R2(d,e,a,b,c,37); R2(c,d,e,a,b,38); R2(b,c,d,e,a,39);
   R3(a,b,c,d,e,40); R3(e,a,b,c,d,41); R3(d,e,a,b,c,42); R3(c,d,e,a,b,43);
   R3(b,c,d,e,a,44); R3(a,b,c,d,e,45); R3(e,a,b,c,d,46); R3(d,e,a,b,c,47);
   R3(c,d,e,a,b,48); R3(b,c,d,e,a,49); R3(a,b,c,d,e,50); R3(e,a,b,c,d,51);
   R3(d,e,a,b,c,52); R3(c,d,e,a,b,53); R3(b,c,d,e,a,54); R3(a,b,c,d,e,55);
   R3(e,a,b,c,d,56); R3(d,e,a,b,c,57); R3(c,d,e,a,b,58); R3(b,c,d,e,a,59);
   R4(a,b,c,d,e,60); R4(e,a,b,c,d,61); R4(d,e,a,b,c,62); R4(c,d,e,a,b,63);
   R4(b,c,d,e,a,64); R4(a,b,c,d,e,65); R4(e,a,b,c,d,66); R4(d,e,a,b,c,67);
   R4(c,d,e,a,b,68); R4(b,c,d,e,a,69); R4(a,b,c,d,e,70); R4(e,a,b,c,d,71);
   R4(d,e,a,b,c,72); R4(c,d,e,a,b,73); R4(b,c,d,e,a,74); R4(a,b,c,d,e,75);
   R4(e,a,b,c,d,76); R4(d,e,a,b,c,77); R4(c,d,e,a,b,78); R4(b,c,d,e,a,79);

   /* Add the working variables back into Context->State[] */
   State[0] += a;
   State[1] += b;
   State[2] += c;
   State[3] += d;
   State[4] += e;

   /* Wipe variables */
   a = b = c = d = e = 0;
}

// /******************************************************************************
 // * A_SHAInit (ntdll.@)
 // *
 // * Initialize a SHA context structure.
 // *
 // * PARAMS
 // *  Context [O] SHA context
 // *
 // * RETURNS
 // *  Nothing
 // */
// void WINAPI A_SHAInit(PSHA_CTX Context)
// {
   // /* SHA1 initialization constants */
   // Context->State[0] = 0x67452301;
   // Context->State[1] = 0xEFCDAB89;
   // Context->State[2] = 0x98BADCFE;
   // Context->State[3] = 0x10325476;
   // Context->State[4] = 0xC3D2E1F0;
   // Context->Count[0] =
   // Context->Count[1] = 0;
// }

// /******************************************************************************
 // * A_SHAUpdate (ntdll.@)
 // *
 // * Update a SHA context with a hashed data from supplied buffer.
 // *
 // * PARAMS
 // *  Context    [O] SHA context
 // *  Buffer     [I] hashed data
 // *  BufferSize [I] hashed data size
 // *
 // * RETURNS
 // *  Nothing
 // */
// void WINAPI A_SHAUpdate(PSHA_CTX Context, const unsigned char *Buffer, UINT BufferSize)
// {
   // ULONG BufferContentSize;

   // BufferContentSize = Context->Count[1] & 63;
   // Context->Count[1] += BufferSize;
   // if (Context->Count[1] < BufferSize)
      // Context->Count[0]++;
   // Context->Count[0] += (BufferSize >> 29);

   // if (BufferContentSize + BufferSize < 64)
   // {
      // memcpy(&Context->Buffer[BufferContentSize], Buffer,
                    // BufferSize);
   // }
   // else
   // {
      // while (BufferContentSize + BufferSize >= 64)
      // {
         // memcpy(Context->Buffer + BufferContentSize, Buffer,
                       // 64 - BufferContentSize);
         // Buffer += 64 - BufferContentSize;
         // BufferSize -= 64 - BufferContentSize;
         // SHA1Transform(Context->State, Context->Buffer);
         // BufferContentSize = 0;
      // }
      // memcpy(Context->Buffer + BufferContentSize, Buffer, BufferSize);
   // }
// }

// /******************************************************************************
 // * A_SHAFinal (ntdll.@)
 // *
 // * Finalize SHA context and return the resulting hash.
 // *
 // * PARAMS
 // *  Context [I/O] SHA context
 // *  Result  [O] resulting hash
 // *
 // * RETURNS
 // *  Nothing
 // */
// void WINAPI A_SHAFinal(PSHA_CTX Context, PULONG Result)
// {
   // INT Pad, Index;
   // UCHAR Buffer[72];
   // ULONG *Count;
   // ULONG BufferContentSize, LengthHi, LengthLo;

   // BufferContentSize = Context->Count[1] & 63;
   // if (BufferContentSize >= 56)
      // Pad = 56 + 64 - BufferContentSize;
   // else
      // Pad = 56 - BufferContentSize;

   // LengthHi = (Context->Count[0] << 3) | (Context->Count[1] >> (32 - 3));
   // LengthLo = (Context->Count[1] << 3);

   // memset(Buffer + 1, 0, Pad - 1);
   // Buffer[0] = 0x80;
   // Count = (ULONG*)(Buffer + Pad);
   // Count[0] = DWORD2BE(LengthHi);
   // Count[1] = DWORD2BE(LengthLo);
   // A_SHAUpdate(Context, Buffer, Pad + 8);

   // for (Index = 0; Index < 5; Index++)
      // Result[Index] = DWORD2BE(Context->State[Index]);

   // memset(Context->Buffer, 0, sizeof(Context->Buffer));
   // A_SHAInit(Context);
// }

/* UNRAVEL should be fastest & biggest */
/* UNROLL_LOOPS should be just as big, but slightly slower */
/* both undefined should be smallest and slowest */

/* Useful defines & typedefs */
typedef unsigned char SHA_BYTE; /* 8-bit quantity */

// SHA_LONG already defined in openssl/sha.h
typedef unsigned int SHA_LONG; /* 32-or-more-bit quantity */
#define SHA_BYTE_ORDER 1234
#define SHA_VERSION 1

#define SHA_BLOCKSIZE 64
#define SHA_DIGESTSIZE 20

typedef struct {
  SHA_LONG digest[5];           /* message digest */
  SHA_LONG count_lo, count_hi;  /* 64-bit bit count */
  SHA_BYTE data[SHA_BLOCKSIZE]; /* SHA data buffer */
  int local;                    /* unprocessed amount in data */
} SHA_INFO;

#define UNRAVEL
/* #define UNROLL_LOOPS */

/* SHA f()-functions */

#define f1(x, y, z) ((x & y) | (~x & z))
#define f2(x, y, z) (x ^ y ^ z)
#define f3(x, y, z) ((x & y) | (x & z) | (y & z))
#define f4(x, y, z) (x ^ y ^ z)

/* SHA constants */

#define CONST1 0x5a827999L
#define CONST2 0x6ed9eba1L
#define CONST3 0x8f1bbcdcL
#define CONST4 0xca62c1d6L

/* truncate to 32 bits -- should be a null op on 32-bit machines */

#define T32(x) ((x)&0xffffffffL)

/* 32-bit rotate */

#define R32(x, n) T32(((x << n) | (x >> (32 - n))))

/* the generic case, for when the overall rotation is not unraveled */

#define FG(n)                                                \
  T = T32(R32(A, 5) + f##n(B, C, D) + E + *WP++ + CONST##n); \
  E = D;                                                     \
  D = C;                                                     \
  C = R32(B, 30);                                            \
  B = A;                                                     \
  A = T

/* specific cases, for when the overall rotation is unraveled */

#define FA(n)                                                \
  T = T32(R32(A, 5) + f##n(B, C, D) + E + *WP++ + CONST##n); \
  B = R32(B, 30)

#define FB(n)                                                \
  E = T32(R32(T, 5) + f##n(A, B, C) + D + *WP++ + CONST##n); \
  A = R32(A, 30)

#define FC(n)                                                \
  D = T32(R32(E, 5) + f##n(T, A, B) + C + *WP++ + CONST##n); \
  T = R32(T, 30)

#define FD(n)                                                \
  C = T32(R32(D, 5) + f##n(E, T, A) + B + *WP++ + CONST##n); \
  E = R32(E, 30)

#define FE(n)                                                \
  B = T32(R32(C, 5) + f##n(D, E, T) + A + *WP++ + CONST##n); \
  D = R32(D, 30)

#define FT(n)                                                \
  A = T32(R32(B, 5) + f##n(C, D, E) + T + *WP++ + CONST##n); \
  C = R32(C, 30)

/* do SHA transformation */

static void ShaTransform(SHA_INFO *sha_info) {
  int i;
  SHA_BYTE *dp;
  SHA_LONG T, A, B, C, D, E, W[80], *WP;

  dp = sha_info->data;

/*
the following makes sure that at least one code block below is
traversed or an error is reported, without the necessity for nested
preprocessor if/else/endif blocks, which are a great pain in the
nether regions of the anatomy...
*/
#undef SWAP_DONE

#if (SHA_BYTE_ORDER == 1234)
#define SWAP_DONE
  for (i = 0; i < 16; ++i) {
    T = *((SHA_LONG *)dp);
    dp += 4;
    W[i] = ((T << 24) & 0xff000000) | ((T << 8) & 0x00ff0000) |
           ((T >> 8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
  }
#endif /* SHA_BYTE_ORDER == 1234 */

#if (SHA_BYTE_ORDER == 4321)
#define SWAP_DONE
  for (i = 0; i < 16; ++i) {
    T = *((SHA_LONG *)dp);
    dp += 4;
    W[i] = T32(T);
  }
#endif /* SHA_BYTE_ORDER == 4321 */

#if (SHA_BYTE_ORDER == 12345678)
#define SWAP_DONE
  for (i = 0; i < 16; i += 2) {
    T = *((SHA_LONG *)dp);
    dp += 8;
    W[i] = ((T << 24) & 0xff000000) | ((T << 8) & 0x00ff0000) |
           ((T >> 8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
    T >>= 32;
    W[i + 1] = ((T << 24) & 0xff000000) | ((T << 8) & 0x00ff0000) |
               ((T >> 8) & 0x0000ff00) | ((T >> 24) & 0x000000ff);
  }
#endif /* SHA_BYTE_ORDER == 12345678 */

#if (SHA_BYTE_ORDER == 87654321)
#define SWAP_DONE
  for (i = 0; i < 16; i += 2) {
    T = *((SHA_LONG *)dp);
    dp += 8;
    W[i] = T32(T >> 32);
    W[i + 1] = T32(T);
  }
#endif /* SHA_BYTE_ORDER == 87654321 */

#ifndef SWAP_DONE
#error Unknown byte order -- you need to add code here
#endif /* SWAP_DONE */

  for (i = 16; i < 80; ++i) {
    W[i] = W[i - 3] ^ W[i - 8] ^ W[i - 14] ^ W[i - 16];
#if (SHA_VERSION == 1)
    W[i] = R32(W[i], 1);
#endif /* SHA_VERSION */
  }
  A = sha_info->digest[0];
  B = sha_info->digest[1];
  C = sha_info->digest[2];
  D = sha_info->digest[3];
  E = sha_info->digest[4];
  WP = W;
#ifdef UNRAVEL
  FA(1);
  FB(1);
  FC(1);
  FD(1);
  FE(1);
  FT(1);
  FA(1);
  FB(1);
  FC(1);
  FD(1);
  FE(1);
  FT(1);
  FA(1);
  FB(1);
  FC(1);
  FD(1);
  FE(1);
  FT(1);
  FA(1);
  FB(1);
  FC(2);
  FD(2);
  FE(2);
  FT(2);
  FA(2);
  FB(2);
  FC(2);
  FD(2);
  FE(2);
  FT(2);
  FA(2);
  FB(2);
  FC(2);
  FD(2);
  FE(2);
  FT(2);
  FA(2);
  FB(2);
  FC(2);
  FD(2);
  FE(3);
  FT(3);
  FA(3);
  FB(3);
  FC(3);
  FD(3);
  FE(3);
  FT(3);
  FA(3);
  FB(3);
  FC(3);
  FD(3);
  FE(3);
  FT(3);
  FA(3);
  FB(3);
  FC(3);
  FD(3);
  FE(3);
  FT(3);
  FA(4);
  FB(4);
  FC(4);
  FD(4);
  FE(4);
  FT(4);
  FA(4);
  FB(4);
  FC(4);
  FD(4);
  FE(4);
  FT(4);
  FA(4);
  FB(4);
  FC(4);
  FD(4);
  FE(4);
  FT(4);
  FA(4);
  FB(4);
  sha_info->digest[0] = T32(sha_info->digest[0] + E);
  sha_info->digest[1] = T32(sha_info->digest[1] + T);
  sha_info->digest[2] = T32(sha_info->digest[2] + A);
  sha_info->digest[3] = T32(sha_info->digest[3] + B);
  sha_info->digest[4] = T32(sha_info->digest[4] + C);
#else /* !UNRAVEL */
#ifdef UNROLL_LOOPS
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(1);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(2);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(3);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
  FG(4);
#else  /* !UNROLL_LOOPS */
  for (i = 0; i < 20; ++i) {
    FG(1);
  }
  for (i = 20; i < 40; ++i) {
    FG(2);
  }
  for (i = 40; i < 60; ++i) {
    FG(3);
  }
  for (i = 60; i < 80; ++i) {
    FG(4);
  }
#endif /* !UNROLL_LOOPS */
  sha_info->digest[0] = T32(sha_info->digest[0] + A);
  sha_info->digest[1] = T32(sha_info->digest[1] + B);
  sha_info->digest[2] = T32(sha_info->digest[2] + C);
  sha_info->digest[3] = T32(sha_info->digest[3] + D);
  sha_info->digest[4] = T32(sha_info->digest[4] + E);
#endif /* !UNRAVEL */
}

void A_SHAInit(SHA_INFO *sha_info) {
  sha_info->digest[0] = 0x67452301L;
  sha_info->digest[1] = 0xefcdab89L;
  sha_info->digest[2] = 0x98badcfeL;
  sha_info->digest[3] = 0x10325476L;
  sha_info->digest[4] = 0xc3d2e1f0L;
  sha_info->count_lo = 0L;
  sha_info->count_hi = 0L;
  sha_info->local = 0;
}

/* update the SHA digest */
void A_SHAUpdate(SHA_INFO *sha_info, SHA_BYTE *buffer, int count) {
  int i;
  SHA_LONG clo;

  clo = T32(sha_info->count_lo + ((SHA_LONG)count << 3));
  if (clo < sha_info->count_lo) {
    ++sha_info->count_hi;
  }
  sha_info->count_lo = clo;
  sha_info->count_hi += (SHA_LONG)count >> 29;
  if (sha_info->local) {
    i = SHA_BLOCKSIZE - sha_info->local;
    if (i > count) {
      i = count;
    }
    memcpy(((SHA_BYTE *)sha_info->data) + sha_info->local, buffer, i);
    count -= i;
    buffer += i;
    sha_info->local += i;
    if (sha_info->local == SHA_BLOCKSIZE) {
      ShaTransform(sha_info);
    } else {
      return;
    }
  }
  while (count >= SHA_BLOCKSIZE) {
    memcpy(sha_info->data, buffer, SHA_BLOCKSIZE);
    buffer += SHA_BLOCKSIZE;
    count -= SHA_BLOCKSIZE;
    ShaTransform(sha_info);
  }
  memcpy(sha_info->data, buffer, count);
  sha_info->local = count;
}

/* finish computing the SHA digest */
void A_SHAFinal(unsigned char digest[20], SHA_INFO *sha_info) {
  int count;
  SHA_LONG lo_bit_count, hi_bit_count;

  lo_bit_count = sha_info->count_lo;
  hi_bit_count = sha_info->count_hi;
  count = (int)((lo_bit_count >> 3) & 0x3f);
  ((SHA_BYTE *)sha_info->data)[count++] = 0x80;
  if (count > SHA_BLOCKSIZE - 8) {
    memset(((SHA_BYTE *)sha_info->data) + count, 0, SHA_BLOCKSIZE - count);
    ShaTransform(sha_info);
    memset((SHA_BYTE *)sha_info->data, 0, SHA_BLOCKSIZE - 8);
  } else {
    memset(((SHA_BYTE *)sha_info->data) + count, 0, SHA_BLOCKSIZE - 8 - count);
  }
  sha_info->data[56] = (unsigned char)((hi_bit_count >> 24) & 0xff);
  sha_info->data[57] = (unsigned char)((hi_bit_count >> 16) & 0xff);
  sha_info->data[58] = (unsigned char)((hi_bit_count >> 8) & 0xff);
  sha_info->data[59] = (unsigned char)((hi_bit_count >> 0) & 0xff);
  sha_info->data[60] = (unsigned char)((lo_bit_count >> 24) & 0xff);
  sha_info->data[61] = (unsigned char)((lo_bit_count >> 16) & 0xff);
  sha_info->data[62] = (unsigned char)((lo_bit_count >> 8) & 0xff);
  sha_info->data[63] = (unsigned char)((lo_bit_count >> 0) & 0xff);
  ShaTransform(sha_info);
  digest[0] = (unsigned char)((sha_info->digest[0] >> 24) & 0xff);
  digest[1] = (unsigned char)((sha_info->digest[0] >> 16) & 0xff);
  digest[2] = (unsigned char)((sha_info->digest[0] >> 8) & 0xff);
  digest[3] = (unsigned char)((sha_info->digest[0]) & 0xff);
  digest[4] = (unsigned char)((sha_info->digest[1] >> 24) & 0xff);
  digest[5] = (unsigned char)((sha_info->digest[1] >> 16) & 0xff);
  digest[6] = (unsigned char)((sha_info->digest[1] >> 8) & 0xff);
  digest[7] = (unsigned char)((sha_info->digest[1]) & 0xff);
  digest[8] = (unsigned char)((sha_info->digest[2] >> 24) & 0xff);
  digest[9] = (unsigned char)((sha_info->digest[2] >> 16) & 0xff);
  digest[10] = (unsigned char)((sha_info->digest[2] >> 8) & 0xff);
  digest[11] = (unsigned char)((sha_info->digest[2]) & 0xff);
  digest[12] = (unsigned char)((sha_info->digest[3] >> 24) & 0xff);
  digest[13] = (unsigned char)((sha_info->digest[3] >> 16) & 0xff);
  digest[14] = (unsigned char)((sha_info->digest[3] >> 8) & 0xff);
  digest[15] = (unsigned char)((sha_info->digest[3]) & 0xff);
  digest[16] = (unsigned char)((sha_info->digest[4] >> 24) & 0xff);
  digest[17] = (unsigned char)((sha_info->digest[4] >> 16) & 0xff);
  digest[18] = (unsigned char)((sha_info->digest[4] >> 8) & 0xff);
  digest[19] = (unsigned char)((sha_info->digest[4]) & 0xff);
}