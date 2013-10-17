/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Macros.h"
#include <iostream>
#include <sstream>
#if defined(ANDROID)
#include <android/log.h>
#endif /* defined(ANDROID) */

namespace Util {
void output(output_priority_t priority, const std::string & message) {
#if defined(ANDROID)
	int androidPriority = ANDROID_LOG_UNKNOWN;
	switch(priority) {
		case OUTPUT_DEBUG:
			androidPriority = ANDROID_LOG_DEBUG;
			break;
		case OUTPUT_WARNING:
			androidPriority = ANDROID_LOG_WARN;
			break;
		case OUTPUT_ERROR:
			androidPriority = ANDROID_LOG_ERROR;
			break;
	}
	__android_log_print(androidPriority, "MobileRendering", "%s", message.c_str());
#else
	std::string type;
	switch(priority) {
		case OUTPUT_DEBUG:
			type = "Debug";
			break;
		case OUTPUT_WARNING:
			type = "Warning";
			break;
		case OUTPUT_ERROR:
			type = "Error";
			break;
		default:
			WARN("unexpected case in switch statement");
	}
	std::cerr << type << ": "<<message<<std::endl;
#endif
}

std::string composeDebugMessage(const std::string & message,const char * file,int line){
	std::ostringstream s;
	s << message << " ("<<file<<":"<<line<<")";
	return s.str();
}
}
