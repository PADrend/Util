/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "Util.h"
#include "IO/ArchiveProvider.h"
#include "IO/FSProvider.h"
#include "IO/DBFSProvider.h"
#include "IO/NetProvider.h"
#include "IO/SerialProvider.h"
#include "IO/ZIPProvider.h"
#include "Serialization/StreamerSTB.h"
#include "GenericAttributeSerialization.h"

#include <iostream>

namespace Util {
using namespace Serialization;

bool init() {
	static int initializationCount = 0;
	if(initializationCount++ == 0) {
		bool result = true;
		{
			// Initialize file system providers
			if(!FSProvider::init()) {
				result = false;
			}
#ifdef UTIL_HAVE_LIB_ARCHIVE
			if(!ArchiveProvider::init()) {
				result = false;
			}
#endif /* UTIL_HAVE_LIB_ARCHIVE */
#ifdef UTIL_HAVE_LIB_SQLITE
			if(!DBFSProvider::init()) {
				result = false;
			}
#endif /* UTIL_HAVE_LIB_SQLITE */
#ifdef UTIL_HAVE_LIB_CURL
			if(!NetProvider::init()) {
				result = false;
			}
#endif /* UTIL_HAVE_LIB_CURL */
#ifdef UTIL_HAVE_LIB_SERIAL
			if(!SerialProvider::init()) {
				result = false;
			}
#endif /* UTIL_HAVE_LIB_SERIAL */
#ifdef UTIL_HAVE_LIB_ZIP
			if(!ZIPProvider::init()) {
				result = false;
			}
#endif /* UTIL_HAVE_LIB_ZIP */
		}
		{
			// Initialize bitmap streamers
			if(!StreamerSTB::init()) {
				result = false;
			}
		}
		{
			if(!GenericAttributeSerialization::init()) {
				result = false;
			}
		}
		return result;
	}
	return true;
}

}
