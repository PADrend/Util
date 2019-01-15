/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef NOISEGENERATOR_H
#define NOISEGENERATOR_H
#include <cstdint>

namespace Util {

/*! Based on noise function by Ken Perlin
	\see http://www.noisemachine.com/talk1/

	Improved Noise reference implementation
	http://mrl.nyu.edu/~perlin/noise/
	@ingroup graphics
*/
class NoiseGenerator{
	public:
		NoiseGenerator(uint32_t seed);
		~NoiseGenerator() {
		}

		/**
		 * Evaluate Perlin's noise function for the given coordinates.
		 *
		 * @note The noise is always zero for integer values.
		 * @param x Input coordinate for the first dimension.
		 * @param y Input coordinate for the second dimension.
		 * @param z Input coordinate for the third dimension.
		 * @return Noise value from [-1.0f, 1.0f].
		 */
		float get(const float x, const float y, const float z) const;
	private:
		uint8_t p[256];
};

}

#endif // NOISEGENERATOR_H
