/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "FontRenderer.h"
#include "Bitmap.h"
#include "PixelAccessor.h"
#include "../Macros.h"
#include "../References.h"
#include <limits>
#include <string>
#include <stdexcept>
#include <tuple>

#if defined(UTIL_HAVE_LIB_FREETYPE)
#include <ft2build.h>
#include FT_FREETYPE_H
#include "../LibRegistry.h"

#endif /* defined(UTIL_HAVE_LIB_FREETYPE) */

namespace Util {

#if defined(UTIL_HAVE_LIB_FREETYPE)
#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define FT_FULL_VERSION_STRING  "Freetype " STR(FREETYPE_MAJOR) "." STR(FREETYPE_MINOR) "." STR(FREETYPE_PATCH) " (www.freetype.org)"

static bool libNameInitailized = [](){	
	Util::LibRegistry::registerLibVersionString("FreeType2",FT_FULL_VERSION_STRING); 
	return true;
}();

struct FontRenderer::Implementation {
	FT_Library library;
	FT_Face face;
};

FontRenderer::FontRenderer(const std::string & fontFile) :
	impl(new Implementation) {

	const auto libError = FT_Init_FreeType(&impl->library);
	if(libError) {
		throw std::runtime_error("Cannot initialize FreeType.");
	}

	const auto faceError = FT_New_Face(impl->library,
									   fontFile.c_str(),
									   0,
									   &impl->face);
	if(faceError == FT_Err_Unknown_File_Format) {
		throw std::runtime_error("Unsupported font file format.");
	} else if(faceError) {
		throw std::runtime_error("Error loading font file.");
	}
}

FontRenderer::~FontRenderer() {
	const auto faceError = FT_Done_Face(impl->face);
	if(faceError) {
		WARN("Cannot destroy Face.");
	}
	const auto libError = FT_Done_FreeType(impl->library);
	if(libError) {
		WARN("Cannot destroy FreeType.");
	}
}

//! Copy the bitmap @p src into the bitmap given by @p dst at position (@p offX, @p offY).
static void drawBitmap(const FT_Bitmap & src, PixelAccessor & dst, uint32_t offX, uint32_t offY) {
	for(int y = 0; y < src.rows; ++y) {
		for(int x = 0; x < src.width; ++x) {
			const uint32_t posX = offX + static_cast<uint32_t>(x);
			const uint32_t posY = offY + static_cast<uint32_t>(y);
			// Read the old value and write the maximum of old and new value.
			const uint8_t oldValue = dst.readSingleValueByte(posX, posY);
			dst.writeSingleValueFloat(posX,posY,std::max(oldValue, src.buffer[y * src.pitch + x])/255.0f);
		}
	}
}

/**
 * Calculate the dimensions of a bitmap needed to store the rendered text.
 * 
 * @return Width, height and vertical position of the baseline cursor
 */
static std::tuple<int, int, int> calculateRenderSizes(const FT_Face & face, const std::u32string & text) {
	auto slot = face->glyph;

	int width = 0;
	int maxAboveBaseline = std::numeric_limits<int>::lowest();
	int maxBelowBaseline = std::numeric_limits<int>::lowest();

	// Caculate the width and height required for the bitmap.
	for(auto it = text.cbegin(); it != text.cend(); ++it) {
		const auto glyphError = FT_Load_Char(face, *it, FT_LOAD_RENDER);
		if(glyphError) {
			// Skip invalid glyphs with a warning.
			WARN("Cannot load font glyph.");
			continue;
		}
		if(it == std::prev(text.cend())) {
			width += slot->bitmap_left + slot->bitmap.width;
		} else {
			width += (slot->advance.x >> 6);
		}
		const auto belowBaseline = slot->bitmap.rows - slot->bitmap_top;
		const auto aboveBaseline = slot->bitmap.rows - belowBaseline;
		maxAboveBaseline = std::max(maxAboveBaseline, aboveBaseline);
		maxBelowBaseline = std::max(maxBelowBaseline, belowBaseline);
	}
	return std::make_tuple(width, maxAboveBaseline + maxBelowBaseline, maxAboveBaseline);
}

Reference<Bitmap> FontRenderer::renderText(unsigned int size, const std::u32string & text) {
	const auto sizeError = FT_Set_Pixel_Sizes(impl->face, size, size);
	if(sizeError) {
		throw std::runtime_error("Cannot set font size.");
	}

	const auto dimensions = calculateRenderSizes(impl->face, text);
	Reference<Bitmap> bitmap = new Bitmap(static_cast<uint32_t>(std::get<0>(dimensions)), 
										  static_cast<uint32_t>(std::get<1>(dimensions)), 
										  PixelFormat::MONO);

	{
		int32_t cursorX = 0;
		const int32_t cursorY = std::get<2>(dimensions);

		auto slot = impl->face->glyph;

		Reference<PixelAccessor> accessor = PixelAccessor::create(bitmap.get());

		for(const auto & character : text) {
			const auto glyphError = FT_Load_Char(impl->face, character, FT_LOAD_RENDER);
			if(glyphError) {
				// Skip invalid glyphs with a warning.
				WARN("Cannot load font glyph.");
				continue;
			}

			drawBitmap(slot->bitmap, 
					   *accessor.get(), 
					   static_cast<uint32_t>(cursorX + slot->bitmap_left), 
					   static_cast<uint32_t>(cursorY - slot->bitmap_top)); 

			cursorX += slot->advance.x >> 6;
		}
		// accessor goes out of scope here
	}
	return std::move(bitmap);
}

/**
 * Calculate the dimensions of a bitmap needed to store the rendered glyphs.
 * 
 * @return Width and height
 */
static std::pair<int, int> calculateGlyphSizes(const FT_Face & face, const std::u32string & chars) {
	auto slot = face->glyph;

	int width = 0;
	int height = 0;

	// Caculate the width and height required for the bitmap.
	for(const auto & character : chars) {
		const auto glyphError = FT_Load_Char(face, character, FT_LOAD_RENDER);
		if(glyphError) {
			// Skip invalid glyphs with a warning.
			WARN("Cannot load font glyph.");
			continue;
		}
		width += slot->bitmap.width;
		height = std::max(height, slot->bitmap.rows);
	}
	return std::make_pair(width, height);
}

std::pair<Reference<Bitmap>, FontInfo> FontRenderer::createGlyphBitmap(unsigned int size, const std::u32string & chars) {
	const auto sizeError = FT_Set_Pixel_Sizes(impl->face, size, size);
	if(sizeError) {
		throw std::runtime_error("Cannot set font size.");
	}

	const auto & metrics = impl->face->size->metrics;
	FontInfo fontInfo;
	fontInfo.ascender = (metrics.ascender >> 6);
	fontInfo.descender = (metrics.descender >> 6);
	fontInfo.height = (metrics.height >> 6);

	auto & glyphMap = fontInfo.glyphMap;

	const auto dimensions = calculateGlyphSizes(impl->face, chars);
	Reference<Bitmap> bitmap = new Bitmap(static_cast<uint32_t>(dimensions.first), 
										  static_cast<uint32_t>(dimensions.second),
										  PixelFormat::MONO);
	{
		int32_t cursorX = 0;
		const int32_t cursorY = 0;

		auto slot = impl->face->glyph;

		Reference<PixelAccessor> accessor = PixelAccessor::create(bitmap.get());

		for(const auto & character : chars) {
			const auto glyphError = FT_Load_Char(impl->face, character, FT_LOAD_RENDER);
			if(glyphError) {
				// Skip invalid glyphs with a warning.
				WARN("Cannot load font glyph.");
				continue;
			}

			drawBitmap(slot->bitmap, 
					   *accessor.get(), 
					   static_cast<uint32_t>(cursorX), 
					   static_cast<uint32_t>(cursorY));

			GlyphInfo gInfo;
			gInfo.position = std::make_pair(cursorX, cursorY);
			gInfo.size = std::make_pair(slot->bitmap.width, slot->bitmap.rows);
			gInfo.offset = std::make_pair(slot->bitmap_left, slot->bitmap_top);
			gInfo.xAdvance = (slot->advance.x >> 6);
			glyphMap.emplace(character, gInfo);

			cursorX += slot->bitmap.width;
		}
		// accessor goes out of scope here
	}
	return std::make_pair(std::move(bitmap), fontInfo);
}

std::map<std::pair<uint32_t,uint32_t>, float> FontRenderer::createKerningMap(const std::u32string & chars){
	std::map<std::pair<uint32_t,uint32_t>, float> kMap; 
	if(FT_HAS_KERNING( impl->face )){
		for(const auto & char1 : chars) {
			const auto index1 = FT_Get_Char_Index( impl->face, char1 );
			if(index1){
				for(const auto & char2 : chars) {
					const auto index2 = FT_Get_Char_Index( impl->face, char2 );
					if(index2){
						FT_Vector delta;
						FT_Get_Kerning( impl->face, index1, index2,FT_KERNING_DEFAULT, &delta );
						if(delta.x!=0)
							kMap[ std::make_pair(char1,char2)] = delta.x >> 6;
					}
				}
			}
		}
	}
	return std::move(kMap);
}

#else /* defined(UTIL_HAVE_LIB_FREETYPE) */

struct FontRenderer::Implementation {
};
FontRenderer::FontRenderer(const std::string &) : impl() {
}
FontRenderer::~FontRenderer() = default;
Reference<Bitmap> FontRenderer::renderText(unsigned int, 
										   const std::u32string &) {
	WARN("Build Util with FreeType to enable FontRenderer.");
	return nullptr;
}
std::pair<Reference<Bitmap>, FontInfo> FontRenderer::createGlyphBitmap(unsigned int,
																	   const std::u32string &) {
	WARN("Build Util with FreeType to enable FontRenderer.");
	return std::make_pair(nullptr, FontInfo());
}

std::map<std::pair<uint32_t,uint32_t>, float> FontRenderer::createKerningMap(const std::u32string &){
	WARN("Build Util with FreeType to enable FontRenderer.");
	return std::map<std::pair<uint32_t,uint32_t>, float>();
}

#endif /* defined(UTIL_HAVE_LIB_FREETYPE) */

}
