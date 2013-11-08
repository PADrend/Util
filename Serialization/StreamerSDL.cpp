/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "StreamerSDL.h"
#include "Serialization.h"
#include "../Factory/Factory.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/BitmapUtils.h"
#include "../IO/FileName.h"
#include "../IO/FileUtils.h"
#include "../IO/TemporaryDirectory.h"
#include "../Macros.h"
#include "../References.h"
#include "../StringUtils.h"
#include <cstddef>
#include <fstream>

#if defined(UTIL_HAVE_LIB_SDL2)
#include <SDL.h>
#endif /* defined(UTIL_HAVE_LIB_SDL2) */

namespace Util {

#if defined(UTIL_HAVE_LIB_SDL2)

Reference<Bitmap> StreamerSDL::loadBitmap(std::istream & input) {
	// Save the bitmap to a file temporarily.
	static TemporaryDirectory tempDir("StreamerSDL");

	FileName fileName(tempDir.getPath());
	do {
		fileName.setFile(StringUtils::createRandomString(16) + ".bmp");
	} while(FileUtils::isFile(fileName));

	std::ofstream fileOutput(fileName.getPath().c_str(), std::ios_base::binary);
	if(!fileOutput.good()) {
		fileOutput.close();
		FileUtils::remove(fileName);
		return nullptr;
	}
	fileOutput << input.rdbuf();
	fileOutput.close();

	SDL_Surface * surface = SDL_LoadBMP(fileName.getPath().c_str());
	if (surface == nullptr) {
		FileUtils::remove(fileName);
		return nullptr;
	}
	auto bitmap = BitmapUtils::createBitmapFromSDLSurface(surface);
	SDL_FreeSurface(surface);

	FileUtils::remove(fileName);

	return std::move(bitmap);
}

bool StreamerSDL::saveBitmap(Bitmap * bitmap, std::ostream & output) {
	if (bitmap == nullptr) {
		return false;
	}
	if(bitmap->getPixelFormat() == PixelFormat::MONO_FLOAT) {
		Reference<Bitmap> tmp = BitmapUtils::convertBitmap(*bitmap, PixelFormat::MONO);
		return saveBitmap(tmp.get(), output);
	}
	SDL_Surface * surface = BitmapUtils::createSDLSurfaceFromBitmap(bitmap);
	if (surface == nullptr) {
		return false;
	}
	// Save the bitmap to a file temporarily.
	static TemporaryDirectory tempDir("StreamerSDL");

	FileName fileName(tempDir.getPath());
	do {
		fileName.setFile(StringUtils::createRandomString(16) + ".bmp");
	} while(FileUtils::isFile(fileName));

	int success = SDL_SaveBMP(surface, fileName.getPath().c_str());
	SDL_FreeSurface(surface);
	if(success != 0) {
		FileUtils::remove(fileName);
		return false;
	}

	std::ifstream fileInput(fileName.getPath().c_str(), std::ios_base::binary);
	output << fileInput.rdbuf();
	fileInput.close();

	FileUtils::remove(fileName);

	return true;
}

#endif /* defined(UTIL_HAVE_LIB_SDL2) */

bool StreamerSDL::init() {
#if defined(UTIL_HAVE_LIB_SDL2)
	Serialization::registerBitmapLoader("bmp", ObjectCreator<StreamerSDL>());
	Serialization::registerBitmapSaver("bmp", ObjectCreator<StreamerSDL>());
#endif /* defined(UTIL_HAVE_LIB_SDL2) */
	return true;
}

}
