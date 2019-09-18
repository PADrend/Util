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
#include "../LibRegistry.h"
#include "../IO/FileUtils.h"
#include "../IO/FileName.h"
#include <limits>
#include <string>
#include <stdexcept>
#include <tuple>
#include <cmath>

#if defined(UTIL_HAVE_LIB_FREETYPE)
#include <ft2build.h>
#include FT_FREETYPE_H
#elif defined(UTIL_HAVE_LIB_STB)
#define STB_TRUETYPE_IMPLEMENTATION
#define STBTT_STATIC
#include <stb_truetype.h>
#endif /* defined(UTIL_HAVE_LIB_FREETYPE) */

#include <iostream>

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
	for(int y = 0; y < static_cast<int>(src.rows); ++y) {
		for(int x = 0; x < static_cast<int>(src.width); ++x) {
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
			width += static_cast<int>(slot->bitmap_left) +static_cast<int>(slot->bitmap.width);
		} else {
			width += (slot->advance.x >> 6);
		}
		const int belowBaseline = static_cast<int>(slot->bitmap.rows) - static_cast<int>(slot->bitmap_top);
		const int aboveBaseline = static_cast<int>(slot->bitmap.rows) - static_cast<int>(belowBaseline);
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
						 static_cast<uint32_t>(cursorX + static_cast<int>(slot->bitmap_left)), 
						 static_cast<uint32_t>(cursorY - static_cast<int>(slot->bitmap_top))); 

			cursorX += slot->advance.x >> 6;
		}
		// accessor goes out of scope here
	}
	return bitmap;
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
		width += static_cast<int>(slot->bitmap.width);
		height = std::max(height, static_cast<int>(slot->bitmap.rows));
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
			gInfo.position = std::make_pair(static_cast<int>(cursorX), static_cast<int>(cursorY));
			gInfo.size = std::make_pair(static_cast<int>(slot->bitmap.width), static_cast<int>(slot->bitmap.rows));
			gInfo.offset = std::make_pair(static_cast<int>(slot->bitmap_left), static_cast<int>(slot->bitmap_top));
			gInfo.xAdvance = (slot->advance.x >> 6);
			glyphMap.emplace(character, gInfo);

			cursorX += static_cast<int>(slot->bitmap.width);
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
	return kMap;
}

#elif defined(UTIL_HAVE_LIB_STB)

static bool libNameInitailized = [](){	
	Util::LibRegistry::registerLibVersionString("stb_truetype","stb_truetype v1.22 (http://github.com/nothings/stb)"); 
	return true;
}();

struct FontRenderer::Implementation {
	stbtt_fontinfo info;
	float scale = 1.0;
	std::vector<uint8_t> data;
};

FontRenderer::FontRenderer(const std::string & fontFile) : impl(new Implementation) {	
	impl->data = FileUtils::loadFile(FileName(fontFile));	
	if(!stbtt_InitFont(&impl->info, impl->data.data(), 0))
		throw std::runtime_error("Cannot initialize font: " + fontFile);
}

FontRenderer::~FontRenderer() = default;

//! Copy the bitmap @p src into the bitmap given by @p dst at position (@p offX, @p offY).
static void drawBitmap(PixelAccessor & src, PixelAccessor & dst, uint32_t w, uint32_t h, uint32_t offX, uint32_t offY) {
	for(int y = 0; y < h; ++y) {
		for(int x = 0; x < w; ++x) {
			const uint32_t posX = offX + static_cast<uint32_t>(x);
			const uint32_t posY = offY + static_cast<uint32_t>(y);
			// Read the old value and write the maximum of old and new value.
			const float oldValue = dst.readSingleValueFloat(posX, posY);
			dst.writeSingleValueFloat(posX,posY,std::max(oldValue, src.readSingleValueFloat(x, y)));
		}
	}
}

/**
 * Calculate the dimensions of a bitmap needed to store the rendered text.
 * 
 * @return Width, height, vertical position of the baseline cursor and and max char width
 */
static std::tuple<int, int, int, int> calculateRenderSizes(const stbtt_fontinfo& info, float scale, const std::u32string & text) {
	int width = 0;
	int ascent, descent;
	stbtt_GetFontVMetrics(&info, &ascent, &descent, 0);    
	int baseline = ascent * scale;
	int height = (ascent - descent) * scale;
	int maxWidth = 0;

	// Caculate the width and height required for the bitmap.
	for(auto it = text.cbegin(); it != text.cend(); ++it) {
		int advance;
		stbtt_GetCodepointHMetrics(&info, *it, &advance, 0);
		width += advance * scale;
		maxWidth = std::max<int>(maxWidth, advance * scale + 1);
		
		if(it != std::prev(text.cend())) {
			int kern;
			kern = stbtt_GetCodepointKernAdvance(&info, *it, *std::next(it));
			width += kern * scale;
		}
	}
	return std::make_tuple(width, height, baseline, maxWidth);
}

Reference<Bitmap> FontRenderer::renderText(unsigned int size, const std::u32string & text) {
	float scale = stbtt_ScaleForPixelHeight(&impl->info, size);
	impl->scale = scale;
	const auto dimensions = calculateRenderSizes(impl->info, scale, text);
	Reference<Bitmap> bitmap = new Bitmap(static_cast<uint32_t>(std::get<0>(dimensions)), 
											static_cast<uint32_t>(std::get<1>(dimensions)), 
											PixelFormat::MONO);
	{
		int32_t cursorX = 0;
		const int32_t cursorY = 0;//std::get<2>(dimensions);
		const int32_t maxWidth = std::get<3>(dimensions);
		Reference<Bitmap> tmpBitmap = new Bitmap(maxWidth, static_cast<uint32_t>(std::get<1>(dimensions)), PixelFormat::MONO);
		
		Reference<PixelAccessor> accessor = PixelAccessor::create(bitmap.get());
		Reference<PixelAccessor> tmpAcc = PixelAccessor::create(tmpBitmap.get());
		
		for(auto it = text.cbegin(); it != text.cend(); ++it) {			
			int x0, y0, x1, y1;
			stbtt_GetCodepointBitmapBox(&impl->info, *it, scale, scale, &x0, &y0, &x1, &y1);
			int w = x1 - x0;
			int h = y1 - y0;
			stbtt_MakeCodepointBitmap(&impl->info, tmpBitmap->data(), w, h, maxWidth, scale, scale, *it);
			
			drawBitmap(*tmpAcc.get(), *accessor.get(), w, h, cursorX, cursorY);
			
			int advance;
			stbtt_GetCodepointHMetrics(&impl->info, *it, &advance, 0);
			cursorX += advance * scale;

			if(it != std::prev(text.cend())) {
				int kern;
				kern = stbtt_GetCodepointKernAdvance(&impl->info, *it, *std::next(it));
				cursorX += kern * scale;
			}
		}
		// accessor goes out of scope here
	}
	return bitmap;
}

std::pair<Reference<Bitmap>, FontInfo> FontRenderer::createGlyphBitmap(unsigned int size, const std::u32string& chars) {	
	float scale = stbtt_ScaleForPixelHeight(&impl->info, size);
	FontInfo fontInfo;
	stbtt_GetFontVMetrics(&impl->info, &fontInfo.ascender, &fontInfo.descender, 0);
	scale = static_cast<float>(size)/fontInfo.ascender;
	fontInfo.ascender *= scale;
	fontInfo.descender *= scale;
	fontInfo.height = fontInfo.ascender - fontInfo.descender;
	auto& glyphMap = fontInfo.glyphMap;
	impl->scale = scale;

	// Caculate the width required for the bitmap.
	int width = 0;
	int maxWidth = 0;
	int padding = 2;	
	for(const auto& character : chars) {
		int advance;
		stbtt_GetCodepointHMetrics(&impl->info, character, &advance, 0);
		width += std::ceil(static_cast<float>(advance) * scale) + padding;
		maxWidth = std::max<int>(maxWidth, advance * scale + 2*padding);
	}

	Reference<Bitmap> bitmap = new Bitmap(width, fontInfo.height, PixelFormat::MONO);

	{
		int cursorX = 0;
		int cursorY = 0;
		Reference<Bitmap> tmpBitmap = new Bitmap(maxWidth, fontInfo.height, PixelFormat::MONO);
		Reference<PixelAccessor> accessor = PixelAccessor::create(bitmap.get());
		Reference<PixelAccessor> tmpAcc = PixelAccessor::create(tmpBitmap.get());
		
		for(const auto& character : chars) {
			int advance, x0, y0, x1, y1;
      int glyph = stbtt_FindGlyphIndex(&impl->info, character);
			if(glyph == 0) {
				// Skip invalid glyphs with a warning.
				WARN("Cannot load font glyph.");
				continue;
			}
			
			stbtt_GetGlyphHMetrics(&impl->info, glyph, &advance, 0);
			stbtt_GetGlyphBitmapBox(&impl->info, glyph, scale, scale, &x0, &y0, &x1, &y1);
			int w = x1 - x0;
			int h = y1 - y0;
			cursorY = 0;			
			stbtt_MakeGlyphBitmap(&impl->info, tmpBitmap->data(), w, h, maxWidth, scale, scale, glyph);
			
			drawBitmap(*tmpAcc.get(), *accessor.get(), w, h, cursorX, cursorY);
			
			GlyphInfo gInfo;
			gInfo.position = std::make_pair(cursorX, cursorY);
			gInfo.size = std::make_pair(w, h);
			gInfo.offset = std::make_pair(x0, -y0);
			gInfo.xAdvance = advance * scale;
			glyphMap.emplace(character, gInfo);
			cursorX += std::ceil(static_cast<float>(advance) * scale) + padding;
		}
	}
	return std::make_pair(std::move(bitmap), fontInfo);
}

std::map<std::pair<uint32_t,uint32_t>, float> FontRenderer::createKerningMap(const std::u32string & chars) {
	std::map<std::pair<uint32_t,uint32_t>, float> kMap; 
	for(const auto & char1 : chars) {
		const auto index1 = stbtt_FindGlyphIndex(&impl->info, char1);
		if(index1){
			for(const auto & char2 : chars) {
				const auto index2 = stbtt_FindGlyphIndex(&impl->info, char2);
				if(index2) {
					int kern = stbtt_GetGlyphKernAdvance(&impl->info, index1, index2);
					kMap[ std::make_pair(char1,char2)] = kern * impl->scale;
				}
			}
		}
	}
	return kMap;
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
