/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef ENCODING_H
#define ENCODING_H

#include <cstdint>
#include <string>
#include <vector>

namespace Util {
	
//! @addtogroup util_helper
//! @{

std::string encodeBase64(const std::vector<uint8_t> & source);
std::vector<uint8_t> decodeBase64(const std::string & source);

//! @}
}
#endif // ENCODING_H
