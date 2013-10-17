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

#ifndef UTIL_CONCURRENCY_POSIXTHREAD_H
#define UTIL_CONCURRENCY_POSIXTHREAD_H

#include "Thread.h"

#include <pthread.h>

namespace Util {
namespace Concurrency {

//! Thread implementation using POSIX threads.
class POSIXThread : public Thread {
	public:
		//! Destructor
		virtual ~POSIXThread();

		bool start(void * (*run)(void *), void * data) override;
		bool join() override;

	protected:
		//! Thread creation is done by the factory function @a createThread().
		POSIXThread();

		//! Allow access to constructor from factory.
		friend Thread * createThread();

	private:
		//! POSIX threads structure representing a thread.
		pthread_t thread;

		//! Determines if the thread has been started and has not been stopped yet.
		bool running;
};

}
}

#endif /* UTIL_CONCURRENCY_POSIXTHREAD_H */

#endif /* defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32) */
