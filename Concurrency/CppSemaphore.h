/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#if not defined(__clang__) and not defined(_WIN32)

#ifndef UTIL_CONCURRENCY_CPPSEMAPHORE_H
#define UTIL_CONCURRENCY_CPPSEMAPHORE_H

#include "Semaphore.h"
#include <condition_variable>
#include <mutex>

namespace Util {
namespace Concurrency {

//! Semaphore implementation using std::condition_variable from C++11.
class CppSemaphore : public Semaphore {
	public:
		//! Destructor
		virtual ~CppSemaphore();

		bool wait() override;
		bool tryWait() override;
		bool post() override;
		uint32_t getValue() override;

	protected:
		//! Semaphore creation is done by the factory method @a createSemaphore().
		CppSemaphore();

		//! Allow access to constructor from factory.
		friend Semaphore * createSemaphore();

	private:
		std::mutex mutex;
		std::condition_variable conditionVariable;
		uint32_t counter;
};

}
}

#endif /* UTIL_CONCURRENCY_CPPSEMAPHORE_H */

#endif /* not defined(__clang__) and not defined(_WIN32) */
