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

#ifndef UTIL_CONCURRENCY_POSIXSEMAPHORE_H
#define UTIL_CONCURRENCY_POSIXSEMAPHORE_H

#include "Semaphore.h"

#include <semaphore.h>

namespace Util {
namespace Concurrency {

//! Semaphore implementation using POSIX threads.
class POSIXSemaphore : public Semaphore {
	public:
		//! Destructor
		virtual ~POSIXSemaphore();

		bool wait() override;
		bool tryWait() override;
		bool post() override;
		uint32_t getValue() override;

	protected:
		//! Semaphore creation is done by the factory method @a createSemaphore().
		POSIXSemaphore();

		//! Allow access to constructor from factory.
		friend Semaphore * createSemaphore();

	private:
		sem_t sem;
};

}
}

#endif /* UTIL_CONCURRENCY_POSIXSEMAPHORE_H */

#endif /* defined(UTIL_HAVE_PTHREAD) and not defined(_WIN32) */
