/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef BITMAPUTILS_H_
#define BITMAPUTILS_H_

#include "../References.h"
#include "PixelFormat.h"
#include <cstddef>
#include <cstdint>
#include <functional>
#include <vector>

namespace Util {
class Bitmap;
class Color4f;
class PixelAccessor;
struct AttributeFormat;

/**
 * Collection of Bitmap related operations.
 * @ingroup graphics
 */
namespace BitmapUtils {

struct BitmapAlteringContext {
	PixelAccessor * pixels;
	uint32_t x;
	uint32_t y;
};
typedef std::function<Color4f (const BitmapAlteringContext &)> BitmapAlteringFunction;

/**
 * Change the content of a Bitmap.
 * 
 * @note this function is designed for flexibility and not for high performance.
 * For real-time applications use a specialized implementation.
 * @param bitmap Bitmap that is to be changed
 * @param op Operation that is called for every pixel of the bitmap
 */
UTILAPI void alterBitmap(Bitmap & bitmap, const BitmapAlteringFunction & op);


//! Blend all given images into one having the given format.
UTILAPI Reference<Bitmap> blendTogether(PixelFormat targetFormat, 
									   const std::vector<Reference<Bitmap> > & sources);

/**
 * Combines all given images into one having the given format.
 * \note first pixel of first bitmap, first pixel of second bitmap, etc...
 */
UTILAPI Reference<Bitmap> combineInterleaved(PixelFormat targetFormat, 
											const std::vector<Reference<Bitmap> > & sources);

/**
 * internal method, used for saving images which are in a format that
 * can't be saved directly because of the limitations of png, bmp, etc...
 *
 * @param source the bitmap to be converted
 * @param newFormat the Pixelformat into which the bitmap schould be converted
 * @return a new bitmap of the specified format with the content of the given bitmap
 */
UTILAPI Reference<Bitmap> convertBitmap(const Bitmap & source, PixelFormat newFormat);

/**
 * internal method, used for expanding the number of channels of a bitmap to 4,
 * e.g., converting RGB to RGBA. The new channels are filled with the last available channel
 * (except the alpha channel, which is set to 1)
 *
 * @param source the bitmap to be converted
 * @param desiredChannels the number of new channels
 * @return a new bitmap of the specified format with the content of the given bitmap
 */
UTILAPI Reference<Bitmap> expandChannels(const Bitmap & source, uint32_t desiredChannels);

/**
 * Create a black/transparent - white bitmap with the given format based on a 
 * bitmask given as raw data. This is used for storing bitmap-font-data
 * directly in a source file.
 */
UTILAPI Reference<Bitmap> createBitmapFromBitMask(const uint32_t width,
												 const uint32_t height,
												 PixelFormat format,
												 const size_t dataSize,
												 const uint8_t * data);

//! Normalizes each pixel to the range [0,1]
UTILAPI void normalizeBitmap(Bitmap & bitmap);

}

}

#endif /* BITMAPUTILS_H_ */
