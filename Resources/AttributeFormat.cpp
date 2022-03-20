/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019-2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#include "AttributeFormat.h"

#include <sstream>

namespace Util {

//------------------

std::string AttributeFormat::toString() const {
	std::ostringstream s;
	s << nameId.toString() << " (off " << offset << "): ";
	auto pixelFormat = Util::toString(getPixelFormat());
	if(pixelFormat == "Unknown") {
		s << static_cast<uint32_t>(components) << "x" << getTypeString(baseType);
		if(normalized)
			s << "n";	
		if(normalized)
			s << "s";	
		if(internalType>0)
			s << "(" << internalType << ")";	
	} else {
		s << pixelFormat;
	}
	return s.str();
}

//------------------

} /* Util */
