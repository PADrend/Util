/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "StreamerPNG.h"
#include "Serialization.h"
#include "../Factory/Factory.h"
#include "../Graphics/Bitmap.h"
#include "../Graphics/BitmapUtils.h"
#include "../Macros.h"
#include "../References.h"
#include <cstddef>
#include <istream>
#include <ostream>
#include <vector>

#ifdef UTIL_HAVE_LIB_PNG
COMPILER_WARN_PUSH
COMPILER_WARN_OFF_GCC(-Wliteral-suffix)
#include <png.h>
COMPILER_WARN_POP
#endif /* UTIL_HAVE_LIB_PNG */

namespace Util {
namespace Serialization {

#ifdef UTIL_HAVE_LIB_PNG

Reference<Bitmap> StreamerPNG::loadBitmap(std::istream & input) {
	char header[8];
	input.read(header, 8);
	const int is_png = !png_sig_cmp(reinterpret_cast<png_byte *>(header), 0, 8);
	if(!is_png) {
		WARN("File is not a valid PNG image.");
		return nullptr;
	}

	// Set up the necessary structures for libpng.
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if(!png_ptr) {
		return nullptr;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if(!info_ptr) {
		png_destroy_read_struct(&png_ptr, static_cast<png_infopp>(nullptr), static_cast<png_infopp>(nullptr));
		return nullptr;
	}

	if(setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_read_struct(&png_ptr, &info_ptr, static_cast<png_infopp>(nullptr));
		return nullptr;
	}

	struct PNGFunctions {
			static void readData(png_structp read_ptr, png_bytep data, png_size_t length) {
				std::istream * in = reinterpret_cast<std::istream *>(png_get_io_ptr(read_ptr));
				if(in == nullptr || !in->good()) {
					png_error(read_ptr, "Error in input stream.");
				}
				in->read(reinterpret_cast<char *>(data), static_cast<std::streamsize>(length));
				if(in->gcount() != static_cast<std::streamsize>(length)) {
					png_error(read_ptr, "Requested amount of data could not be extracted from input stream");
				}
			}
	};

	png_set_read_fn(png_ptr, reinterpret_cast<png_voidp>(&input), PNGFunctions::readData);

	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width;
	png_uint_32 height;
	int bit_depth;
	int color_type;
	png_get_IHDR(	png_ptr, info_ptr,
					&width, &height,
					&bit_depth, &color_type, nullptr, nullptr, nullptr);

	auto pixelFormat = PixelFormat::RGB;
	switch(color_type) {
		case PNG_COLOR_TYPE_GRAY:
			// Convert bpp less than 8 to 8 bits.
			if(bit_depth < 8) {
				png_set_expand_gray_1_2_4_to_8(png_ptr);
			}
			pixelFormat = PixelFormat::MONO;
			break;
		case PNG_COLOR_TYPE_GRAY_ALPHA:
			// Convert grayscale with alpha to RGBA.
			png_set_expand(png_ptr);
			png_set_gray_to_rgb(png_ptr);
			pixelFormat = PixelFormat::RGBA;
			break;
		case PNG_COLOR_TYPE_PALETTE:
			// Convert color palette to RGB(A).
			png_set_expand(png_ptr);

			// Check if the color palette contains transparent colors.
#if PNG_LIBPNG_VER >= 10300
			if(png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)){
				pixelFormat = PixelFormat::RGBA;
			}
#else
			if(info_ptr->valid & PNG_INFO_tRNS) {
				pixelFormat = PixelFormat::RGBA;
			}
#endif
			break;
		case PNG_COLOR_TYPE_RGB_ALPHA:
			pixelFormat = PixelFormat::RGBA;
			break;
		default:
			// Already set to RGB above.
			break;
	}
	// Convert 16 bpp to 8 bits.
	if (bit_depth == 16) {
		png_set_strip_16(png_ptr);
	}

	// Create the bitmap to store the data.
	Reference<Bitmap> bitmap = new Bitmap(width, height, pixelFormat);

	auto row_pointers = new png_bytep[height];
	const uint8_t bytes = pixelFormat.getBytesPerPixel();
	for (uint_fast32_t row = 0; row < height; ++row) {
		// Take over rows in the same order.
		row_pointers[row] = reinterpret_cast<png_bytep>(bitmap->data() + row * width * bytes);
	}

	// This function automatically handles interlacing.
	png_read_image(png_ptr, row_pointers);

	png_read_end(png_ptr, nullptr);
	png_destroy_read_struct(&png_ptr, &info_ptr, static_cast<png_infopp>(nullptr));
	png_free_data(png_ptr, info_ptr, PNG_FREE_ALL, -1);
	delete [] row_pointers;

	return bitmap;
}

bool StreamerPNG::saveBitmap(const Bitmap & bitmap, std::ostream & output) {
	volatile int colorType = 0; // volatile is needed because of the setjmp later on.
	volatile int transforms = 0;

	const auto& pixelFormat = bitmap.getPixelFormat();
	if(pixelFormat == PixelFormat::RGBA) {
		colorType = PNG_COLOR_TYPE_RGB_ALPHA;
		transforms = PNG_TRANSFORM_IDENTITY;
	} else if(pixelFormat == PixelFormat::BGRA) {
		colorType = PNG_COLOR_TYPE_RGB_ALPHA;
		transforms = PNG_TRANSFORM_BGR;
	} else if(pixelFormat == PixelFormat::RGB) {
		colorType = PNG_COLOR_TYPE_RGB;
		transforms = PNG_TRANSFORM_IDENTITY;
	} else if(pixelFormat == PixelFormat::BGR) {
		colorType = PNG_COLOR_TYPE_RGB;
		transforms = PNG_TRANSFORM_BGR;
	} else if(pixelFormat == PixelFormat::MONO) {
		colorType = PNG_COLOR_TYPE_GRAY;
		transforms = PNG_TRANSFORM_IDENTITY;
	} else if(pixelFormat == PixelFormat::MONO_FLOAT) {
		Reference<Bitmap> tmp = BitmapUtils::convertBitmap(bitmap, PixelFormat::MONO);
		return saveBitmap(*tmp.get(), output);
	} else {
		WARN("Unable to save PNG file. Unsupported color type.");
		return false;
	}

	// Set up the necessary structures for libpng.
	png_structp png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (!png_ptr) {
		return false;
	}

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (!info_ptr) {
		png_destroy_write_struct(&png_ptr, nullptr);
		return false;
	}
	if (setjmp(png_jmpbuf(png_ptr))) {
		png_destroy_write_struct(&png_ptr, &info_ptr);
		return false;
	}

	struct PNGFunctions {
			static void writeData(png_structp write_ptr, png_bytep data, png_size_t length) {
				std::ostream * out = reinterpret_cast<std::ostream *>(png_get_io_ptr(write_ptr));
				out->write(reinterpret_cast<const char*>(data), static_cast<std::streamsize>(length));
			}
			static void flushData(png_structp flush_ptr) {
				std::ostream * out = reinterpret_cast<std::ostream *>(png_get_io_ptr(flush_ptr));
				out->flush();
			}
	};

	png_set_write_fn(png_ptr, reinterpret_cast<png_voidp>(&output), PNGFunctions::writeData, PNGFunctions::flushData);

	const uint32_t width = bitmap.getWidth();
	const uint32_t height = bitmap.getHeight();

	png_set_IHDR(png_ptr, info_ptr, width, height, 8, colorType, PNG_INTERLACE_NONE, PNG_COMPRESSION_TYPE_DEFAULT, PNG_FILTER_TYPE_DEFAULT);

	// Write the image.
	std::vector<png_bytep> row_pointers;
	row_pointers.reserve(height);
	const uint8_t bytes = pixelFormat.getDataSize();
	for (uint_fast32_t row = 0; row < height; ++row) {
		// Take over rows in the same order.
		row_pointers.push_back(reinterpret_cast<png_bytep>(const_cast<uint8_t *>(bitmap.data()) + row * width * bytes));
	}
	png_set_rows(png_ptr, info_ptr, row_pointers.data());

	png_write_png(png_ptr, info_ptr, transforms, nullptr);

	// Clean up.
	png_destroy_write_struct(&png_ptr, &info_ptr);

	return true;
}

#endif /* UTIL_HAVE_LIB_PNG */

bool StreamerPNG::init() {
#ifdef UTIL_HAVE_LIB_PNG
	Serialization::registerBitmapLoader("png", ObjectCreator<StreamerPNG>());
	Serialization::registerBitmapSaver("png", ObjectCreator<StreamerPNG>());
#endif /* UTIL_HAVE_LIB_PNG */
	return true;
}

}
}
