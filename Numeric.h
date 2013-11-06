/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef NUMERIC_H_
#define NUMERIC_H_

#include <algorithm>
#include <cmath>
#include <cstdint>
#include <limits>

namespace Util {
namespace Numeric {

/**
 * Test if the two parameters are essentially equal as defined in
 * the given reference by Knuth.
 *
 * @param u First floating point parameter.
 * @param v Second floating point parameter.
 * @return @c true if both floating point values are essentially
 * equal, @c false otherwise.
 * @see Donald E. Knuth: The art of computer programming.
 * Volume II: Seminumerical algorithms. Addison-Wesley, 1969.
 * Page 128, Equation (24).
 */
inline bool equal(const float u, const float v) {
	return std::abs(v - u) <= std::numeric_limits<float>::epsilon()
			* std::min(std::abs(u), std::abs(v));
}

/**
 * originally from: http://www.zeiner.at/c/Matrix.html
 *
 * Calculates inverse of matrix a. a must be a matrix of size nx2n and
 * the matrix which should be inverted is on the left side. Unit vectors
 * on the right side will be added by function.
 *
 *    matrix a                                   matrix a^(-1)
 *   a11 a12 a13  | 1  0  0        1   0   0   | b11 b12 b13
 *   a21 a22 a23  | 0  1  0   =>   0   1   0   | b21 b22 b23
 *   a31 a32 a33  | 0  0  1        0   0   1   | b31 b32 b33
 *
 * @param a matrix to be inverted
 * @param n size of matrix nx2n
 * @return true if inversion was successful
 */
inline bool invertMatrix(float * a, uint16_t n) {
	const float epsilon = 0.0001f; // Genauigkeit
	const uint_fast16_t rowSize = 2u * n;

	// Set right half of matrix to identity.
	for (uint_fast16_t i = 0; i < n; ++i) {
		for (uint_fast16_t j = 0; j < n; ++j) {
			const uint_fast16_t elementOffset = i * rowSize + n + j;
			if (i == j) {
				a[elementOffset] = 1.0f;
			} else {
				a[elementOffset] = 0.0f;
			}
		}
	}

	// gauss-jordan-algorithm
	for(uint_fast16_t s = 0; s < n; ++s) {
		const uint32_t rowOffsetS = s * rowSize;

		// Pivotisierung vermeidet unn�tigen Abbruch bei einer Null in der Diagnonalen und
		// erh�ht die Rechengenauigkeit
		float maximum = std::abs(a[rowOffsetS + s]);

		uint_fast16_t prow = s;
		for (uint_fast16_t i = s+1; i < n; ++i) {
			const uint32_t rowOffsetI = i * rowSize;
			if (std::abs(a[rowOffsetI + s]) > maximum) {
				maximum = std::abs(a[rowOffsetI + s]);
				prow = i;
			}
		}

		if(maximum < epsilon) {
			return false; // not invertible
		}

		if (prow != s){ // swap rows if necessary
			for (uint_fast16_t j = s; j < rowSize; ++j){
				float h = a[s * rowSize + j];
				a[s * rowSize + j] = a[prow * rowSize + j];
				a[prow * rowSize + j]= h;
			}
		}

		// divide row by pivot coefficient f = a[s][s]
		float f = a[rowOffsetS + s];
		for (uint_fast16_t j = s; j < rowSize; ++j) {
			a[rowOffsetS + j] /= f;
		}

		// elimination: make column s a unit vector by adding row s to row i
		for (uint_fast16_t i = 0; i < n; ++i){
			if (i != s){
				const uint32_t rowOffsetI = i * rowSize;
				f = -a[rowOffsetI + s];
				for (uint_fast16_t j = s; j < rowSize; ++j) {
					a[rowOffsetI + j] += f*a[rowOffsetS + j];
				}
			}
		}
	}

	return true;
}

}
}

#endif /* NUMERIC_H_ */
