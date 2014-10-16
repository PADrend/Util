/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_STREAMERPNG_H_
#define UTIL_STREAMERPNG_H_

#include "AbstractBitmapStreamer.h"
#include "../References.h"
#include <cstdint>
#include <iosfwd>
#include <string>

namespace Util {
class Bitmap;

class StreamerPNG : public AbstractBitmapStreamer {
	public:
		StreamerPNG() :
			AbstractBitmapStreamer() {
		}
		virtual ~StreamerPNG() {
		}

		Reference<Bitmap> loadBitmap(std::istream & input) override;
		bool saveBitmap(const Bitmap & bitmap, std::ostream & output) override;

		static bool init();
};
}

#endif /* UTIL_STREAMERPNG_H_ */
