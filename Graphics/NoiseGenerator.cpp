/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "NoiseGenerator.h"
#include <cmath>
#include <random>
#include <utility>

namespace Util {

static float fade(const float t) {
	// 6 * t^5 - 15 * t^4 + 10 * t^3
	// = ((6 * t - 15) * t) + 10) * t^3
	return t * t * t * (t * (t * 6 - 15) + 10);
}
static float lerp(const float t, const float a, const float b) {
	return a + t * (b - a);
}
static float grad(const uint8_t hash, const float x, const float y, const float z) {
	static const float gradients[16][3] = {
		/*  0 = 0000 */	{ 1,  1,  0},
		/*  1 = 0001 */	{-1,  1,  0},
		/*  2 = 0010 */	{ 1, -1,  0},
		/*  3 = 0011 */	{-1, -1,  0},
		/*  4 = 0100 */	{ 1,  0,  1},
		/*  5 = 0101 */	{-1,  0,  1},
		/*  6 = 0110 */	{ 1,  0, -1},
		/*  7 = 0111 */	{-1,  0, -1},
		/*  8 = 1000 */	{ 0,  1,  1},
		/*  9 = 1001 */	{ 0, -1,  1},
		/* 10 = 1010 */	{ 0,  1, -1},
		/* 11 = 1011 */	{ 0, -1, -1},
		/* 12 = 1100 */	{ 1,  1,  0},
		/* 13 = 1101 */	{ 0, -1,  1},
		/* 14 = 1110 */	{-1,  1,  0},
		/* 15 = 1111 */	{ 0, -1, -1}
	};
	const float * g = gradients[hash & 0x0f];
	return g[0] * x + g[1] * y + g[2] * z;
// 	uint32_t h = hash & 15;                      // CONVERT LO 4 BITS OF HASH CODE
// 	float u = h<8 ? x : y;                 	// INTO 12 GRADIENT DIRECTIONS.
// 	float v = h<4 ? y : h==12||h==14 ? x : z;
// 	return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

NoiseGenerator::NoiseGenerator(uint32_t seed) {
	for(uint_fast16_t i = 0; i < 256; ++i) {
		p[i] = i;
	}
	std::default_random_engine engine(seed);
	for(uint_fast8_t i = 0; i < 255; ++i) {
		std::uniform_int_distribution<uint_fast8_t> distribution(0, 255 - i);
		const uint_fast8_t offset = distribution(engine);
		using std::swap;
		swap(p[i], p[i + offset]);
	}
}

float NoiseGenerator::get(const float _x, const float _y, const float _z) const {
	// FIND UNIT CUBE THAT CONTAINS POINT.
	const uint8_t X = static_cast<uint32_t>(std::floor(_x)) & 255;
	const uint8_t Y = static_cast<uint32_t>(std::floor(_y)) & 255;
	const uint8_t Z = static_cast<uint32_t>(std::floor(_z)) & 255;

	// FIND RELATIVE X,Y,Z OF POINT IN CUBE.
	const float x = _x - std::floor(_x);
	const float y = _y - std::floor(_y);
	const float z = _z - std::floor(_z);

	// COMPUTE FADE CURVES FOR EACH OF X,Y,Z.
	const float u = fade(x);
	const float v = fade(y);
	const float w = fade(z);

	// HASH COORDINATES OF THE 8 CUBE CORNERS,
	const uint16_t A 	= p[X] + Y;
	const uint16_t AA 	= p[A % 256] + Z;
	const uint16_t AB 	= p[(A + 1) % 256] + Z;
	const uint16_t B 	= p[(X + 1) % 256] + Y;
	const uint16_t BA 	= p[B % 256] + Z;
	const uint16_t BB 	= p[(B + 1) % 256] + Z;

	return lerp(w,
				lerp(v,
					lerp(u,
						grad(p[AA % 256],       x    , y    , z    ),		// AND ADD
						grad(p[BA % 256],       x - 1, y    , z    )),		// BLENDED
					lerp(u,
						grad(p[AB % 256],       x    , y - 1, z    ),		// RESULTS
						grad(p[BB % 256],       x - 1, y - 1, z    ))),		// FROM  8
				lerp(v,
					lerp(u,
						grad(p[(AA + 1) % 256], x    , y    , z - 1),		// CORNERS
						grad(p[(BA + 1) % 256], x - 1, y    , z - 1)),		// OF CUBE
					lerp(u,
						grad(p[(AB + 1) % 256], x    , y - 1, z - 1),
						grad(p[(BB + 1) % 256], x - 1, y - 1, z - 1))));
}

}
