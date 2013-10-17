/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_ABSTRACTBITMAPSTREAMER_H_
#define UTIL_ABSTRACTBITMAPSTREAMER_H_

#include "AbstractStreamer.h"
#include "../Macros.h"
#include <istream>
#include <ostream>
#include <cstdint>
#include <string>

namespace Util {
class Bitmap;

/**
 * Interface for classes that are capable of converting between bitmaps and streams.
 *
 * @author Benjamin Eikel
 * @date 2011-09-08
 */
class AbstractBitmapStreamer : public AbstractStreamer {
	public:
		virtual ~AbstractBitmapStreamer() {
		}

		/**
		 * Load a bitmap from the given stream.
		 *
		 * @param input Use the data from the stream beginning at the preset position.
		 * @return Bitmap object. The caller is responsible for the memory deallocation.
		 */
		virtual Bitmap * loadBitmap(std::istream & /*input*/) {
			WARN("Unsupported call for loading a single bitmap.");
			return nullptr;
		}

		/**
		 * Save a bitmap to the given stream.
		 *
		 * @param bitmap Bitmap object to save.
		 * @param output Use the stream for writing beginning at the preset position.
		 * @return @c true if successful, @c false otherwise.
		 */
		virtual bool saveBitmap(Bitmap * /*bitmap*/, std::ostream & /*output*/) {
			WARN("Unsupported call for saving a single bitmap.");
			return false;
		}

	protected:
		//! Creation is only possible in subclasses.
		AbstractBitmapStreamer() : AbstractStreamer() {
		}
};

}

#endif /* UTIL_ABSTRACTBITMAPSTREAMER_H_ */
