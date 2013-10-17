/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32)

#include "POSIXMutex.h"

namespace Util {
namespace Concurrency {

POSIXMutex::POSIXMutex() :
	Mutex() {
	pthread_mutex_init(&mutex, nullptr);
}

POSIXMutex::~POSIXMutex() {
	pthread_mutex_destroy(&mutex);
}

bool POSIXMutex::lock() {
	return (pthread_mutex_lock(&mutex) == 0);
}

bool POSIXMutex::unlock() {
	return (pthread_mutex_unlock(&mutex) == 0);
}

}
}

#endif /* defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32) */
