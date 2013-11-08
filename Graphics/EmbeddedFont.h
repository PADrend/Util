/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_GRAPHICS_EMBEDDEDFONT_H
#define UTIL_GRAPHICS_EMBEDDEDFONT_H

#include "../References.h"
#include <utility>

namespace Util {
class Bitmap;
struct FontInfo;

//! Single font embedded in source code
namespace EmbeddedFont {

/**
 * Return the default font that is embedded into the source code. A font size 
 * of 8 pixels was used to generate the data.
 * 
 * @remark The font "pf_tempesta_seven_condensed.ttf" by Yusuke Kamiyamane was
 * used to generate the data.
 * @li License: "Diese Schriftarten dürfen unentgeltlich für persönliche und
 * kommerzielle Projekte verwendet werden."
 * @li URL: http://p.yusukekamiyamane.com/fonts/
 * 
 * @see Util::FontRenderer::createGlyphBitmap for a description of the return
 * value
 */
std::pair<Reference<Bitmap>, FontInfo> getFont();

}
}

#endif /* UTIL_GRAPHICS_EMBEDDEDFONT_H */
