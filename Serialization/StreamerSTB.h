/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_STREAMERSTB_H_
#define UTIL_STREAMERSTB_H_

#include "AbstractBitmapStreamer.h"
#include "../References.h"
#include <cstdint>
#include <iosfwd>
#include <string>

namespace Util {
class Bitmap;
namespace Serialization {

class StreamerSTB : public AbstractBitmapStreamer {
	public:
		StreamerSTB() :
			AbstractBitmapStreamer() {
		}
		virtual ~StreamerSTB() {
		}

		UTILAPI Reference<Bitmap> loadBitmap(std::istream & input) override;
		UTILAPI bool saveBitmap(const Bitmap & bitmap, std::ostream & output) override;

		UTILAPI static bool init();
};

}
}

#endif /* UTIL_STREAMERSTB_H_ */
