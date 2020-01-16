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
#include "StreamerSTB.h"
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
#include "../LibRegistry.h"
#include <cstddef>
#include <fstream>

#if defined(UTIL_HAVE_LIB_STB) and not defined(UTIL_PREFER_SDL_IMAGE)
#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image.h>
#include <stb_image_write.h>

static bool libNameInitailized = [](){	
	Util::LibRegistry::registerLibVersionString("stb_image","stb_image v2.19; stb_image_write v1.09 (http://github.com/nothings/stb)"); 
	return true;
}();

#endif /* defined(UTIL_HAVE_LIB_STB) */

namespace Util {
namespace Serialization {

#if defined(UTIL_HAVE_LIB_STB) and not defined(UTIL_PREFER_SDL_IMAGE)

Reference<Bitmap> StreamerSTB::loadBitmap(std::istream & input) {
	input.seekg(0, std::ios::end);
	std::streampos size = input.tellg();
	input.seekg(0, std::ios::beg);

	std::vector<uint8_t> data(size);
	input.read(reinterpret_cast<char *>(data.data()), size);
	
	int width,height,components;
	uint8_t* img = stbi_load_from_memory(data.data(), size, &width, &height, &components, 0);
	if(!img) {
		WARN(std::string("Could not create image. ") + stbi_failure_reason());
		return nullptr;
	}
	auto pixelFormat = PixelFormat::RGB;
	switch (components) {
		case STBI_grey:
			pixelFormat = PixelFormat::MONO;
			break;
		case STBI_grey_alpha:
			pixelFormat = PixelFormat::RG;
			break;
		case STBI_rgb:
			pixelFormat = PixelFormat::RGB;
			break;
		case STBI_rgb_alpha:
			pixelFormat = PixelFormat::RGBA;
			break;
	}

	// Create the bitmap to store the data.
	Reference<Bitmap> bitmap = new Bitmap(width, height, pixelFormat);
	std::copy(img, img + width*height*components, bitmap->data());	
	stbi_image_free(img);
	return bitmap;
}

struct WriteContext {
	WriteContext(std::ostream& out) : output(out) {}
	std::ostream& output;
};

static void writeData(void* ctx, void* data, int size) {
	auto* context = static_cast<WriteContext*>(ctx);
	context->output.write(static_cast<const char*>(data), size);
}

bool StreamerSTB::saveBitmap(const Bitmap & bitmap, std::ostream & output) {
	WriteContext context(output);
	const auto& pixelFormat = bitmap.getPixelFormat();
	int components = bitmap.getHeight();
	const int width = bitmap.getWidth();
	const int height = bitmap.getHeight();
	const int stride = pixelFormat.getBytesPerPixel() * width;
	if(pixelFormat == PixelFormat::RGBA) {
		components = 4;
	} else if(pixelFormat == PixelFormat::BGRA) {
		Reference<Bitmap> tmp = BitmapUtils::convertBitmap(bitmap, PixelFormat::RGBA);
		return saveBitmap(*tmp.get(), output);
	} else if(pixelFormat == PixelFormat::RGB) {
		components = 3;
	} else if(pixelFormat == PixelFormat::BGR) {
		Reference<Bitmap> tmp = BitmapUtils::convertBitmap(bitmap, PixelFormat::RGB);
		return saveBitmap(*tmp.get(), output);
	} else if(pixelFormat == PixelFormat::RG) {
		components =2;
	} else if(pixelFormat == PixelFormat::MONO) {
		components = 1;
	} else if(pixelFormat == PixelFormat::MONO_FLOAT) {
		Reference<Bitmap> tmp = BitmapUtils::convertBitmap(bitmap, PixelFormat::MONO);
		return saveBitmap(*tmp.get(), output);
	} else {
		WARN("Unable to save PNG file. Unsupported color format.");
		return false;
	}
	return stbi_write_png_to_func(writeData, &context, width, height, components, bitmap.data(), stride) != 0;
}

#endif /* defined(UTIL_HAVE_LIB_STB) */

bool StreamerSTB::init() {
#if defined(UTIL_HAVE_LIB_STB) and not defined(UTIL_PREFER_SDL_IMAGE)
	static const std::string fileExtensions[] = { "jpeg", "jpg", "png", "tga", "bmp", "psd", "gif", "hdr", "pic", "pnm" };
	for(auto & fileExtension : fileExtensions)
		Serialization::registerBitmapLoader(fileExtension, ObjectCreator<StreamerSTB>());
	Serialization::registerBitmapSaver("png", ObjectCreator<StreamerSTB>());
#endif /* defined(UTIL_HAVE_LIB_STB) */
	return true;
}

}
}
