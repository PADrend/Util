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

struct ColorLibrary{
	static const Color4ub BLACK;
	static const Color4ub WHITE;
	static const Color4ub RED;
	static const Color4ub GREEN;
	static const Color4ub BLUE;
	static const Color4ub YELLOW;
	static const Color4ub CYAN;
	static const Color4ub MAGENTA;
	static const Color4ub LIGHT_GREY;
	static const Color4ub DARK_GREY;
	static const Color4ub GRAY;
};

}
#endif // COLOR_LIBRARY_H
