/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef BITMAP_H_
#define BITMAP_H_

#include "../ReferenceCounter.h"
#include "PixelFormat.h"
#include <cstddef>
#include <cstdint>
#include <vector>

//! @defgroup graphics Graphics

namespace Util {

/*! Bitmap

	Coordinates:

	(0,0)                  (width,0)
		  +---------------+
		  |      /\       |
		  |     /  \      |
		  |      ||       |
		  |      ||       |
		  +---------------+
	(0,height)            (width,height)

	\note the coordinates are the same as for SDL Surfaces, but not for OpenGl Textures!
	@ingroup graphics
*/
class Bitmap : public ReferenceCounter<Bitmap> {
public:
	//! Create a new bitmap.
	UTILAPI explicit Bitmap(const uint32_t width = 0, const uint32_t height = 0, PixelFormat pixelFormat = PixelFormat::RGBA8UInt);

	/*! Create a new bitmap which containing only raw data. A direct pixel access is not possible.
			\note This can e.g. be used to store compressed textures */
	UTILAPI Bitmap(const uint32_t width, const uint32_t height, size_t rawDataSize);

	//! Create a copy of the bitmap together with its data.
	UTILAPI explicit Bitmap(const Bitmap& source);

	//! Swap all the data with another bitmap
	UTILAPI void swap(Bitmap& other);

	uint32_t getWidth() const { return width; }
	uint32_t getHeight() const { return height; }

	PixelFormat getPixelFormat() const { return pixelFormat; }

	//!	Return the number of bytes that are allocated by this Bitmap or that will be allocated.
	size_t getDataSize() const { return pixelData.size(); }

	//! Access the data of the bitmap.
	uint8_t* data() { return pixelData.data(); }

	//! Access the data of the bitmap.
	const uint8_t* data() const { return pixelData.data(); }

	/**
		 * Overwrite the current data with the given data.
		 * 
		 * @param newData Data that will be copied to the internal data storage
		 * @note The new data must have the same size as the existing data.
		 */
	UTILAPI void setData(const std::vector<uint8_t>& newData);
	UTILAPI void swapData(std::vector<uint8_t>& other);

	//!	Swap the rows, so that the bitmap is turned upside down afterwards.
	UTILAPI void flipVertically();

private:
	PixelFormat pixelFormat;

	uint32_t width;	 //!< Horizontal size
	uint32_t height; //!< Vertical size

	std::vector<uint8_t> pixelData; //!< Storage of bitmap data
};

} // namespace Util

#endif /* BITMAP_H_ */
