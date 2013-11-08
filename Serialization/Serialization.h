/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_SERIALIZATION_H
#define UTIL_SERIALIZATION_H

#include "../References.h"
#include <functional>
#include <ostream>
#include <string>

namespace Util {
class AbstractBitmapStreamer;
class Bitmap;
class FileName;

/**
 * Conversion between objects and streams.
 * There are static functions for
 * - loading a single bitmap from a stream, and
 * - saving a single bitmap to a stream.
 *
 * @author Benjamin Eikel
 * @date 2011-09-08
 */
namespace Serialization {

/**
 * Load a single bitmap from the given address.
 * The type of the bitmap is determined by the file extension.
 *
 * @param file Address to the file containing the bitmap data.
 * @return A single bitmap.
 */
Reference<Bitmap> loadBitmap(const FileName & url);

/**
 * Create a single bitmap from the given data.
 * The type of the bitmap has to be given as parameter.
 *
 * @param extension File extension specifying the type of the bitmap.
 * @param data Bitmap data.
 * @return A single bitmap.
 */
Reference<Bitmap> loadBitmap(const std::string & extension, const std::string & data);

/**
 * Write a single bitmap to the given address.
 * The type of the bitmap is determined by the file extension.
 *
 * @param bitmap Bitmap object to save.
 * @param file Address to the file that shall be written.
 * @return @c true if successful, @c false otherwise.
 */
bool saveBitmap(const Bitmap & bitmap, const FileName & url);

/**
 * Write a single bitmap to the given stream.
 * The type of the bitmap has to be given as parameter.
 *
 * @param bitmap Bitmap object to save.
 * @param extension File extension specifying the type of the bitmap.
 * @param output Stream to which the data shall be written.
 * @return @c true if successful, @c false otherwise.
 */
bool saveBitmap(const Bitmap & bitmap, const std::string & extension, std::ostream & output);

/**
 * Register a new streamer for the given file extension that supports loading of bitmaps.
 *
 * @param extension File extension specifying the type of the bitmap.
 * @param loaderCreator A function that creates a new streamer when called.
 * @retval @c true if the streamer was registered successfully.
 * @retval @c false if another streamer for the extension already existed.
 */
bool registerBitmapLoader(const std::string & extension, std::function<AbstractBitmapStreamer * ()> loaderCreator);

/**
 * Register a new streamer for the given file extension that supports saving of bitmaps.
 *
 * @param extension File extension specifying the type of the bitmap.
 * @param saverCreator A function that creates a new streamer when called.
 * @retval @c true if the streamer was registered successfully.
 * @retval @c false if another streamer for the extension already existed.
 */
bool registerBitmapSaver(const std::string & extension, std::function<AbstractBitmapStreamer * ()> saverCreator);

}
}

#endif // UTIL_SERIALIZATION_H
