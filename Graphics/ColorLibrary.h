/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef COLOR_LIBRARY_H
#define COLOR_LIBRARY_H

#include "Color.h"

namespace Util{
	
//! @ingroup graphics
struct ColorLibrary{
	UTILAPI static const Color4ub BLACK;
	UTILAPI static const Color4ub WHITE;
	UTILAPI static const Color4ub RED;
	UTILAPI static const Color4ub GREEN;
	UTILAPI static const Color4ub BLUE;
	UTILAPI static const Color4ub YELLOW;
	UTILAPI static const Color4ub CYAN;
	UTILAPI static const Color4ub MAGENTA;
	UTILAPI static const Color4ub LIGHT_GREY;
	UTILAPI static const Color4ub DARK_GREY;
	UTILAPI static const Color4ub GRAY;

	UTILAPI static const Color4ub VIVID_YELLOW;
	UTILAPI static const Color4ub STRONG_PURPLE;
	UTILAPI static const Color4ub VIVID_ORANGE;
	UTILAPI static const Color4ub VERY_LIGHT_BLUE;
	UTILAPI static const Color4ub VIVID_RED;
	UTILAPI static const Color4ub GRAYISH_YELLOW;
	UTILAPI static const Color4ub MEDIUM_GRAY;
	UTILAPI static const Color4ub VIVID_GREEN;
	UTILAPI static const Color4ub STRONG_PURPLISH_PINK;
	UTILAPI static const Color4ub STRONG_BLUE;
	UTILAPI static const Color4ub STRONG_YELLOWISH_PINK;
	UTILAPI static const Color4ub STRONG_VIOLET;
	UTILAPI static const Color4ub VIVID_ORANGE_YELLOW;
	UTILAPI static const Color4ub STRONG_PURPLISH_RED;
	UTILAPI static const Color4ub VIVID_GREENISH_YELLOW;
	UTILAPI static const Color4ub STRONG_REDDISH_BROWN;
	UTILAPI static const Color4ub VIVID_YELLOWISH_GREEN;
	UTILAPI static const Color4ub DEEP_YELLOWISH_BROWN;
	UTILAPI static const Color4ub VIVID_REDDISH_ORANGE;
	UTILAPI static const Color4ub DARK_OLIVE_GREEN;

	UTILAPI static const uint32_t NUM_HIGH_CONTRAST_COLORS;
	UTILAPI static const Color4ub* HIGH_CONTRAST_COLORS;
	static inline Color4ub getHighContrastColor(uint32_t index) { return HIGH_CONTRAST_COLORS[index % NUM_HIGH_CONTRAST_COLORS]; }
};

}
#endif // COLOR_LIBRARY_H
