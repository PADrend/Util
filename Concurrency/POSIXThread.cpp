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

#include "POSIXThread.h"

namespace Util {
namespace Concurrency {

POSIXThread::POSIXThread() : Thread(), running(false) {
}

POSIXThread::~POSIXThread() {
#if not defined(ANDROID)
	if(running) {
		pthread_cancel(thread);
		pthread_join(thread, nullptr);
	}
#endif
}

bool POSIXThread::start(void * (*run)(void *), void * data) {
	pthread_create(&thread, nullptr, run, data);
	running = true;
	return true;
}

bool POSIXThread::join() {
	pthread_join(thread, nullptr);
	running = false;
	return true;
}

}
}

#endif /* defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32) */
