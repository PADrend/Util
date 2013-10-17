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

#ifndef UTIL_CONCURRENCY_CPPMUTEX_H
#define UTIL_CONCURRENCY_CPPMUTEX_H

#include "Mutex.h"
#include <mutex>

namespace Util {
namespace Concurrency {

//! Mutex implementation using std::mutex from C++11.
class CppMutex : public Mutex {
	public:
		//! Destructor
		virtual ~CppMutex();

		bool lock() override;
		bool unlock() override;

	protected:
		//! Mutex creation is done by the factory method @a createMutex().
		CppMutex();

		//! Allow access to constructor from factory.
		friend Mutex * createMutex();

	private:
		std::mutex mutex;
};

}
}

#endif /* UTIL_CONCURRENCY_CPPMUTEX_H */

#endif /* not defined(__clang__) and not defined(_WIN32) */
