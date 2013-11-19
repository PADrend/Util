/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "Cursor.h"
#include "../Graphics/Bitmap.h"

namespace Util {
namespace UI {

Cursor::Cursor(Reference<Bitmap>  image, unsigned int hotSpotX, unsigned int hotSpotY) :
	_image(std::move(image)), _hotSpotX(hotSpotX), _hotSpotY(hotSpotY) {
}

Cursor::~Cursor() = default;

}
}
