/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "ColorLibrary.h"

namespace Util {

// For more colors, see: http://en.wikipedia.org/wiki/Web_colors
const Color4ub ColorLibrary::BLACK(0x00, 0x00, 0x00, 0xFF);
const Color4ub ColorLibrary::WHITE(0xFF, 0xFF, 0xFF, 0xFF);
const Color4ub ColorLibrary::RED(0xFF, 0x00, 0x00, 0xFF);
const Color4ub ColorLibrary::GREEN(0x00, 0xFF, 0x00, 0xFF);
const Color4ub ColorLibrary::BLUE(0x00, 0x00, 0xFF, 0xFF);
const Color4ub ColorLibrary::YELLOW(0xFF, 0xFF, 0x00, 0xFF);
const Color4ub ColorLibrary::CYAN(0x00, 0xFF, 0xFF, 0xFF);
const Color4ub ColorLibrary::MAGENTA(0xFF, 0x00, 0xFF, 0xFF);
const Color4ub ColorLibrary::LIGHT_GREY(0xC0, 0xC0, 0xC0, 0xFF);
const Color4ub ColorLibrary::GRAY(0x80, 0x80, 0x80, 0xFF);
const Color4ub ColorLibrary::DARK_GREY(0x40, 0x40, 0x40, 0xFF);

// High contrast color list by K. Kelly, Color Eng., 3(6) (1965)
const Color4ub ColorLibrary::VIVID_YELLOW(0xFF, 0xB3, 0x00, 0xFF);
const Color4ub ColorLibrary::STRONG_PURPLE(0x80, 0x3E, 0x75, 0xFF);
const Color4ub ColorLibrary::VIVID_ORANGE(0xFF, 0x68, 0x00, 0xFF);
const Color4ub ColorLibrary::VERY_LIGHT_BLUE(0xA6, 0xBD, 0xD7, 0xFF);
const Color4ub ColorLibrary::VIVID_RED(0xC1, 0x00, 0x20, 0xFF);
const Color4ub ColorLibrary::GRAYISH_YELLOW(0xCE, 0xA2, 0x62, 0xFF);
const Color4ub ColorLibrary::MEDIUM_GRAY(0x81, 0x70, 0x66, 0xFF);
const Color4ub ColorLibrary::VIVID_GREEN(0x00, 0x7D, 0x34, 0xFF);
const Color4ub ColorLibrary::STRONG_PURPLISH_PINK(0xF6, 0x76, 0x8E, 0xFF);
const Color4ub ColorLibrary::STRONG_BLUE(0x00, 0x53, 0x8A, 0xFF);
const Color4ub ColorLibrary::STRONG_YELLOWISH_PINK(0xFF, 0x7A, 0x5C, 0xFF);
const Color4ub ColorLibrary::STRONG_VIOLET(0x53, 0x37, 0x7A, 0xFF);
const Color4ub ColorLibrary::VIVID_ORANGE_YELLOW(0xFF, 0x8E, 0x00, 0xFF);
const Color4ub ColorLibrary::STRONG_PURPLISH_RED(0xB3, 0x28, 0x51, 0xFF);
const Color4ub ColorLibrary::VIVID_GREENISH_YELLOW(0xF4, 0xC8, 0x00, 0xFF);
const Color4ub ColorLibrary::STRONG_REDDISH_BROWN(0x7F, 0x18, 0x0D, 0xFF);
const Color4ub ColorLibrary::VIVID_YELLOWISH_GREEN(0x93, 0xAA, 0x00, 0xFF);
const Color4ub ColorLibrary::DEEP_YELLOWISH_BROWN(0x59, 0x33, 0x15, 0xFF);
const Color4ub ColorLibrary::VIVID_REDDISH_ORANGE(0xF1, 0x3A, 0x13, 0xFF);
const Color4ub ColorLibrary::DARK_OLIVE_GREEN(0x23, 0x2C, 0x16, 0xFF);

static const Color4ub HIGH_CONTRAST_COLORS_DATA[] = {
	ColorLibrary::VIVID_YELLOW,
	ColorLibrary::STRONG_PURPLE,
	ColorLibrary::VIVID_ORANGE,
	ColorLibrary::VERY_LIGHT_BLUE,
	ColorLibrary::VIVID_RED,
	ColorLibrary::GRAYISH_YELLOW,
	ColorLibrary::MEDIUM_GRAY,
	ColorLibrary::VIVID_GREEN,
	ColorLibrary::STRONG_PURPLISH_PINK,
	ColorLibrary::STRONG_BLUE,
	ColorLibrary::STRONG_YELLOWISH_PINK,
	ColorLibrary::STRONG_VIOLET,
	ColorLibrary::VIVID_ORANGE_YELLOW,
	ColorLibrary::STRONG_PURPLISH_RED,
	ColorLibrary::VIVID_GREENISH_YELLOW,
	ColorLibrary::STRONG_REDDISH_BROWN,
	ColorLibrary::VIVID_YELLOWISH_GREEN,
	ColorLibrary::DEEP_YELLOWISH_BROWN,
	ColorLibrary::VIVID_REDDISH_ORANGE,
	ColorLibrary::DARK_OLIVE_GREEN
};

const uint32_t ColorLibrary::NUM_HIGH_CONTRAST_COLORS = 22;
const Color4ub* ColorLibrary::HIGH_CONTRAST_COLORS = HIGH_CONTRAST_COLORS_DATA;
}
