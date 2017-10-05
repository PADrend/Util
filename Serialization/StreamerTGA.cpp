/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
  Copyright (C) 2017 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "StreamerTGA.h"
#include "Serialization.h"
#include "../Factory/Factory.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/BitmapUtils.h"
#include "../Macros.h"
#include <cstddef>
#include <istream>

#if defined(UTIL_HAVE_LIB_SDL2) and defined(UTIL_HAVE_LIB_SDL2_IMAGE)
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wswitch-default)
#include <SDL.h>
#include <SDL_image.h>
COMPILER_WARN_POP
#endif /* defined(UTIL_HAVE_LIB_SDL2) and defined(UTIL_HAVE_LIB_SDL2_IMAGE) */

namespace Util {

#if defined(UTIL_HAVE_LIB_SDL2) and defined(UTIL_HAVE_LIB_SDL2_IMAGE)

Reference<Bitmap> StreamerTGA::loadBitmap(std::istream & input) {
	input.seekg(0, std::ios::end);
	std::streampos size = input.tellg();
	input.seekg(0, std::ios::beg);

	std::vector<uint8_t> data(size);
	input.read(reinterpret_cast<char *>(data.data()), size);

	// IMG_Load_RW cannot handle tga files because they have no "magic" identifier
	SDL_Surface * surface = IMG_LoadTyped_RW(SDL_RWFromConstMem(data.data(), data.size()), true, "TGA");
	if (surface == nullptr) {
		WARN(std::string("Could not create image. ") + IMG_GetError());
		return nullptr;
	}
	auto bitmap = BitmapUtils::createBitmapFromSDLSurface(surface);
	SDL_FreeSurface(surface);
	return bitmap;
}

#endif /* defined(UTIL_HAVE_LIB_SDL2) and defined(UTIL_HAVE_LIB_SDL2_IMAGE) */

bool StreamerTGA::init() {
#if defined(UTIL_HAVE_LIB_SDL2) and defined(UTIL_HAVE_LIB_SDL2_IMAGE)
	Serialization::registerBitmapLoader("tga", ObjectCreator<StreamerTGA>());
#endif /* defined(UTIL_HAVE_LIB_SDL2) and defined(UTIL_HAVE_LIB_SDL2_IMAGE) */
	return true;
}

}
