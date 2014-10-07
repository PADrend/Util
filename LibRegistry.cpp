/*
	This file is part of the Util library.
	Copyright (C) 2014 Claudius Jähn <claudius@uni-paderborn.de>

	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "LibRegistry.h"

namespace Util {
namespace LibRegistry {

static std::map<std::string,std::string>& getLibVersionRegistry(){
	static std::map<std::string,std::string> m;
	return m;
}
void registerLibVersionString(std::string libName,  std::string versionString ){
	getLibVersionRegistry()[libName] = versionString;
}
const std::map<std::string,std::string>& getLibVersionStrings(){
	return  getLibVersionRegistry();
}

}
}
