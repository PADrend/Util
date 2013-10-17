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

#include "POSIXSpinLock.h"

namespace Util {
namespace Concurrency {

POSIXSpinLock::POSIXSpinLock() :
	SpinLock() {
	pthread_spin_init(&spinLock, PTHREAD_PROCESS_PRIVATE);
}

POSIXSpinLock::~POSIXSpinLock() {
	pthread_spin_destroy(&spinLock);
}

bool POSIXSpinLock::lock() {
	return (pthread_spin_lock(&spinLock) == 0);
}

bool POSIXSpinLock::unlock() {
	return (pthread_spin_unlock(&spinLock) == 0);
}

}
}

#endif /* defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32) */
