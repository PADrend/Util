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

#include "POSIXSemaphore.h"

namespace Util {
namespace Concurrency {

POSIXSemaphore::POSIXSemaphore() :
	Semaphore() {
	sem_init(&sem, 0, 0);
}

POSIXSemaphore::~POSIXSemaphore() {
	sem_destroy(&sem);
}

bool POSIXSemaphore::wait() {
	return (sem_wait(&sem) == 0);
}

bool POSIXSemaphore::tryWait() {
	return (sem_trywait(&sem) == 0);
}

bool POSIXSemaphore::post() {
	return (sem_post(&sem) == 0);
}

uint32_t POSIXSemaphore::getValue() {
	int value;
	sem_getvalue(&sem, &value);
	return value;
}

}
}

#endif /* defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32) */
