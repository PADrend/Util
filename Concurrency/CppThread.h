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

#ifndef UTIL_CONCURRENCY_CPPTHREAD_H
#define UTIL_CONCURRENCY_CPPTHREAD_H

#include "Thread.h"
#include <thread>

namespace Util {
namespace Concurrency {

//! Thread implementation using std::thread from C++11.
class CppThread : public Thread {
	public:
		//! Destructor
		virtual ~CppThread();

		bool start(void * (*run)(void *), void * data) override;
		bool join() override;

	protected:
		//! Thread creation is done by the factory function @a createThread().
		CppThread();

		//! Allow access to constructor from factory.
		friend Thread * createThread();

	private:
		//! C++11 threads structure representing a thread.
		std::thread thread;
};

}
}

#endif /* UTIL_CONCURRENCY_CPPTHREAD_H */

#endif /* not defined(__clang__) and not defined(_WIN32) */
