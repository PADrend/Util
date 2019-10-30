/*
	This file is part of the Util library.
	Copyright (C) 2014 Claudius Jähn <claudius@uni-paderborn.de>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_LIBREGISTRY_H_INCLUDED
#define UTIL_LIBREGISTRY_H_INCLUDED

#include <map>
#include <string>

namespace Util {
	
//! @ingroup registry
namespace LibRegistry {

/*! Call once for each used third-party library. 
	The information is used for version outputs or licensing infos. */
void registerLibVersionString(std::string libName,  std::string versionString );
const std::map<std::string,std::string>& getLibVersionStrings();
}
}
#endif // UTIL_LIBREGISTRY_H_INCLUDED
