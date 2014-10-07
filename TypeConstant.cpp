/*
	This file is part of the Util library.
	Copyright (C) 2014 Claudius Jähn <claudius@uni-paderborn.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "TypeConstant.h"
#include <stdexcept>

static const uint8_t byteSizes[] = { 1,2,4,8,1,2,4,8,4,8 };
	
size_t Util::getNumBytes(TypeConstant t){
	const uint8_t index = static_cast<uint8_t>(t);
	if( index>=sizeof(byteSizes) )
		throw std::invalid_argument("getNumBytes: Invalid type constant.");
	return byteSizes[index];
}
