/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_GRAPHICS_FONTRENDERER_H
#define UTIL_GRAPHICS_FONTRENDERER_H

#include "../References.h"
#include <memory>
#include <string>
#include <utility>
#include <unordered_map>
#include <map>

namespace Util {
class Bitmap;

struct GlyphInfo {
	//! Pixel position of the glyph inside the bitmap.
	std::pair<int, int> position;
	//! Pixel dimensions of the glyph inside the bitmap.
	std::pair<int, int> size;
	//! Pixel offset to the cursor for rendering the glyph.
	std::pair<int, int> offset;
	//! Pixel offset to advance the cursor after rendering the glyph.
	int xAdvance;
};
struct FontInfo {
	//! Number of pixels above the baseline.
	int ascender;
	//! Number of pixels below the baseline.
	int descender;
	//! Number of pixels between two consecutive baselines.
	int height;
	//! Mapping from characters to their glyph information.
	std::unordered_map<char32_t, GlyphInfo> glyphMap;
};

/**
 * @brief Font rendering
 * 
 * Create a bitmap that contains specific glyphs of a font.
 * 
 * @author Benjamin Eikel
 * @date 2013-07-10
 * @ingroup graphics
 */
class FontRenderer {
	private:
		// Use Pimpl idiom
		struct Implementation;
		std::unique_ptr<Implementation> impl;

	public:
		//! Load the font given as parameter.
		FontRenderer(const std::string & fontFile);

		//! Release the allocated font resources.
		~FontRenderer();

		/**
		 * Render the given text into a bitmap.
		 * 
		 * @param size Font size in pixels
		 * @param text Text to render
		 * @return Bitmap containing the rendered text
		 */
		Reference<Bitmap> renderText(unsigned int size, const std::u32string & text);

		/**
		 * Render the given characters into a bitmap. Create a map from the
		 * characters to a description of their glyph images inside the bitmap.
		 * 
		 * @param size Font size in pixels
		 * @param chars Characters to render
		 * @return Bitmap containing the rendered characters and a structure
		 * containing metrics of the font together with a mapping from the
		 * characters to their glyph information.
		 */
		std::pair<Reference<Bitmap>, FontInfo> createGlyphBitmap(unsigned int size,
																 const std::u32string & chars);

		std::map<std::pair<uint32_t,uint32_t>, float> createKerningMap(const std::u32string & chars);
};

}

#endif /* UTIL_GRAPHICS_FONTRENDERER_H */
