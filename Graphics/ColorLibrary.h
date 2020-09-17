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
};

}
#endif // COLOR_LIBRARY_H
