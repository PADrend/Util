/*
	This file is part of the Rendering library.
	Copyright (C) 2007-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Hashing.h"
#include <cstring>
#include <sstream>
#include <iomanip>

namespace Util {

uint32_t calcHash(const uint8_t * ptr,size_t size){
	uint32_t h = 0;
	for(const uint8_t * end = ptr+size; ptr<end; ++ptr)
		h ^= (((*ptr + h) * 1234393) % 0xffffff);
	return h;

}

//====================================================

/*
 * Public domain implementation of the RSA Data Security, Inc. MD5 Message-Digest Algorithm (RFC 1321).
 * Based on: http://openwall.info/wiki/people/solar/software/public-domain-source-code/md5
 * Author: Alexander Peslyak
 * Modified by: Sascha Brandt
 */
 
struct MD5_Context {
	uint32_t lo = 0;
	uint32_t hi = 0;
	uint32_t a = 0x67452301;
	uint32_t b = 0xefcdab89;
	uint32_t c = 0x98badcfe;
	uint32_t d = 0x10325476;
	uint8_t buffer[64];
	uint32_t block[16];
};
 
// The basic MD5 functions.
#define F(x, y, z)		((z) ^ ((x) & ((y) ^ (z))))
#define G(x, y, z)		((y) ^ ((z) & ((x) ^ (y))))
#define H(x, y, z)		(((x) ^ (y)) ^ (z))
#define H2(x, y, z)		((x) ^ ((y) ^ (z)))
#define I(x, y, z)		((y) ^ ((x) | ~(z)))
 
// The MD5 transformation for all four rounds.
#define STEP(f, a, b, c, d, x, t, s) \
	(a) += f((b), (c), (d)) + (x) + (t); \
	(a) = (((a) << (s)) | (((a) & 0xffffffffu) >> (32 - (s)))); \
	(a) += (b);
 
/*
 * SET reads 4 input bytes in little-endian byte order and stores them in a
 * properly aligned word in host byte order.
 */
#define SET(n) (*reinterpret_cast<const uint32_t*>(&ptr[(n) * 4]))
#define GET(n) SET(n)
 
/*
 * This processes one or more 64-byte data blocks, but does NOT update the bit
 * counters.  There are no alignment requirements.
 */
static const uint8_t* md5_body(MD5_Context& ctx, const uint8_t* data, size_t size) {
	uint32_t a, b, c, d;
	uint32_t saved_a, saved_b, saved_c, saved_d; 
	const uint8_t* ptr = data;
 
	a = ctx.a;
	b = ctx.b;
	c = ctx.c;
	d = ctx.d;
 
	do {
		saved_a = a;
		saved_b = b;
		saved_c = c;
		saved_d = d;
 
/* Round 1 */
		STEP(F, a, b, c, d,  SET(0), 0xd76aa478u, 7)
		STEP(F, d, a, b, c,  SET(1), 0xe8c7b756u, 12)
		STEP(F, c, d, a, b,  SET(2), 0x242070dbu, 17)
		STEP(F, b, c, d, a,  SET(3), 0xc1bdceeeu, 22)
		STEP(F, a, b, c, d,  SET(4), 0xf57c0fafu, 7)
		STEP(F, d, a, b, c,  SET(5), 0x4787c62au, 12)
		STEP(F, c, d, a, b,  SET(6), 0xa8304613u, 17)
		STEP(F, b, c, d, a,  SET(7), 0xfd469501u, 22)
		STEP(F, a, b, c, d,  SET(8), 0x698098d8u, 7)
		STEP(F, d, a, b, c,  SET(9), 0x8b44f7afu, 12)
		STEP(F, c, d, a, b, SET(10), 0xffff5bb1u, 17)
		STEP(F, b, c, d, a, SET(11), 0x895cd7beu, 22)
		STEP(F, a, b, c, d, SET(12), 0x6b901122u, 7)
		STEP(F, d, a, b, c, SET(13), 0xfd987193u, 12)
		STEP(F, c, d, a, b, SET(14), 0xa679438eu, 17)
		STEP(F, b, c, d, a, SET(15), 0x49b40821u, 22)
 
/* Round 2 */
		STEP(G, a, b, c, d,  GET(1), 0xf61e2562u, 5)
		STEP(G, d, a, b, c,  GET(6), 0xc040b340u, 9)
		STEP(G, c, d, a, b, GET(11), 0x265e5a51u, 14)
		STEP(G, b, c, d, a,  GET(0), 0xe9b6c7aau, 20)
		STEP(G, a, b, c, d,  GET(5), 0xd62f105du, 5)
		STEP(G, d, a, b, c, GET(10), 0x02441453u, 9)
		STEP(G, c, d, a, b, GET(15), 0xd8a1e681u, 14)
		STEP(G, b, c, d, a,  GET(4), 0xe7d3fbc8u, 20)
		STEP(G, a, b, c, d,  GET(9), 0x21e1cde6u, 5)
		STEP(G, d, a, b, c, GET(14), 0xc33707d6u, 9)
		STEP(G, c, d, a, b,  GET(3), 0xf4d50d87u, 14)
		STEP(G, b, c, d, a,  GET(8), 0x455a14edu, 20)
		STEP(G, a, b, c, d, GET(13), 0xa9e3e905u, 5)
		STEP(G, d, a, b, c,  GET(2), 0xfcefa3f8u, 9)
		STEP(G, c, d, a, b,  GET(7), 0x676f02d9u, 14)
		STEP(G, b, c, d, a, GET(12), 0x8d2a4c8au, 20)
 
/* Round 3 */
		STEP( H, a, b, c, d,  GET(5), 0xfffa3942u, 4)
		STEP(H2, d, a, b, c,  GET(8), 0x8771f681u, 11)
		STEP( H, c, d, a, b, GET(11), 0x6d9d6122u, 16)
		STEP(H2, b, c, d, a, GET(14), 0xfde5380cu, 23)
		STEP( H, a, b, c, d,  GET(1), 0xa4beea44u, 4)
		STEP(H2, d, a, b, c,  GET(4), 0x4bdecfa9u, 11)
		STEP( H, c, d, a, b,  GET(7), 0xf6bb4b60u, 16)
		STEP(H2, b, c, d, a, GET(10), 0xbebfbc70u, 23)
		STEP( H, a, b, c, d, GET(13), 0x289b7ec6u, 4)
		STEP(H2, d, a, b, c,  GET(0), 0xeaa127fau, 11)
		STEP( H, c, d, a, b,  GET(3), 0xd4ef3085u, 16)
		STEP(H2, b, c, d, a,  GET(6), 0x04881d05u, 23)
		STEP( H, a, b, c, d,  GET(9), 0xd9d4d039u, 4)
		STEP(H2, d, a, b, c, GET(12), 0xe6db99e5u, 11)
		STEP( H, c, d, a, b, GET(15), 0x1fa27cf8u, 16)
		STEP(H2, b, c, d, a,  GET(2), 0xc4ac5665u, 23)
 
/* Round 4 */
		STEP(I, a, b, c, d,  GET(0), 0xf4292244u, 6)
		STEP(I, d, a, b, c,  GET(7), 0x432aff97u, 10)
		STEP(I, c, d, a, b, GET(14), 0xab9423a7u, 15)
		STEP(I, b, c, d, a,  GET(5), 0xfc93a039u, 21)
		STEP(I, a, b, c, d, GET(12), 0x655b59c3u, 6)
		STEP(I, d, a, b, c,  GET(3), 0x8f0ccc92u, 10)
		STEP(I, c, d, a, b, GET(10), 0xffeff47du, 15)
		STEP(I, b, c, d, a,  GET(1), 0x85845dd1u, 21)
		STEP(I, a, b, c, d,  GET(8), 0x6fa87e4fu, 6)
		STEP(I, d, a, b, c, GET(15), 0xfe2ce6e0u, 10)
		STEP(I, c, d, a, b,  GET(6), 0xa3014314u, 15)
		STEP(I, b, c, d, a, GET(13), 0x4e0811a1u, 21)
		STEP(I, a, b, c, d,  GET(4), 0xf7537e82u, 6)
		STEP(I, d, a, b, c, GET(11), 0xbd3af235u, 10)
		STEP(I, c, d, a, b,  GET(2), 0x2ad7d2bbu, 15)
		STEP(I, b, c, d, a,  GET(9), 0xeb86d391u, 21)
 
		a += saved_a;
		b += saved_b;
		c += saved_c;
		d += saved_d;
 
		ptr += 64;
	} while (size -= 64);
 
	ctx.a = a;
	ctx.b = b;
	ctx.c = c;
	ctx.d = d;
 
	return ptr;
}
 
static void md5_update(MD5_Context& ctx, const uint8_t* data, size_t size) {
	uint32_t saved_lo;
	uint32_t used, available;
 
	saved_lo = ctx.lo;
	if ((ctx.lo = (saved_lo + size) & 0x1fffffffu) < saved_lo)
		ctx.hi++;
	ctx.hi += size >> 29;
 
	used = saved_lo & 0x3f;
 
	if(used) {
		available = 64 - used;
 
		if(size < available) {
			memcpy(&ctx.buffer[used], data, size);
			return;
		}
 
		memcpy(&ctx.buffer[used], data, available);
		data += available;
		size -= available;
		md5_body(ctx, ctx.buffer, 64);
	}
 
	if(size >= 64) {
		data = md5_body(ctx, data, size & ~0x3fu);
		size &= 0x3f;
	}
 
	memcpy(ctx.buffer, data, size);
}
 
#define OUT_MD5(dst, src) \
	(dst)[0] = static_cast<uint8_t>(src); \
	(dst)[1] = static_cast<uint8_t>((src) >> 8); \
	(dst)[2] = static_cast<uint8_t>((src) >> 16); \
	(dst)[3] = static_cast<uint8_t>((src) >> 24);

static inline uint32_t reverseOrder(uint32_t value) {
	return ((value & 0xffu) << 24) | ((value & 0xff00u) << 8) | ((value & 0xff0000u) >> 8) | ((value & 0xff000000u) >> 24);
}

//-----------------------------

std::string md5(const std::string& str) {
	MD5_Context ctx;
	md5_update(ctx, reinterpret_cast<const uint8_t*>(str.c_str()), str.length());
	
	// finalize 
	uint32_t used = ctx.lo & 0x3f;
 
	ctx.buffer[used++] = 0x80;
 
	uint32_t available = 64 - used;
 
	if (available < 8) {
		memset(&ctx.buffer[used], 0, available);
		md5_body(ctx, ctx.buffer, 64);
		used = 0;
		available = 64;
	}
 
	memset(&ctx.buffer[used], 0, available - 8);
 
	ctx.lo <<= 3;
	OUT_MD5(&ctx.buffer[56], ctx.lo)
	OUT_MD5(&ctx.buffer[60], ctx.hi)
 
	md5_body(ctx, ctx.buffer, 64);
	
	std::ostringstream ss;
	ss << std::hex << std::setfill('0');
	ss << std::setw(8) << reverseOrder(ctx.a);
	ss << std::setw(8) << reverseOrder(ctx.b);
	ss << std::setw(8) << reverseOrder(ctx.c);
	ss << std::setw(8) << reverseOrder(ctx.d);
	return ss.str();
}

} /* Util */