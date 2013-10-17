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

}
