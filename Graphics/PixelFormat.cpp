/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "PixelFormat.h"

namespace Util {
const PixelFormat PixelFormat::RGB(1, 0,1,2,PixelFormat::NONE);
const PixelFormat PixelFormat::RGB_FLOAT(4, 0,4,8,PixelFormat::NONE);
const PixelFormat PixelFormat::BGR(1, 2,1,0,PixelFormat::NONE);
const PixelFormat PixelFormat::BGR_FLOAT(4, 8,4,0,PixelFormat::NONE);
const PixelFormat PixelFormat::RGBA(1, 0,1,2,3);
const PixelFormat PixelFormat::RGBA_FLOAT(4, 0,4,8,12);
const PixelFormat PixelFormat::BGRA(1, 2,1,0,3);
const PixelFormat PixelFormat::BGRA_FLOAT(4, 8,4,0,12);
const PixelFormat PixelFormat::MONO(1, 0,PixelFormat::NONE,PixelFormat::NONE,PixelFormat::NONE);
const PixelFormat PixelFormat::MONO_FLOAT(4, 0,PixelFormat::NONE,PixelFormat::NONE,PixelFormat::NONE);
const PixelFormat PixelFormat::UNKNOWN(0, PixelFormat::NONE,PixelFormat::NONE,PixelFormat::NONE,PixelFormat::NONE);
}
