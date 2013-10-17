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

#include "CppSemaphore.h"
#include <condition_variable>
#include <mutex>

namespace Util {
namespace Concurrency {

CppSemaphore::CppSemaphore() :
	Semaphore(), mutex(), conditionVariable(), counter(0) {
}

CppSemaphore::~CppSemaphore() {
}

bool CppSemaphore::wait() {
	std::unique_lock<std::mutex> _lock(mutex);
	while(counter == 0) {
		conditionVariable.wait(_lock);
	}
	--counter;
	return true;
}

bool CppSemaphore::tryWait() {
	std::unique_lock<std::mutex> _lock(mutex);
	if(counter == 0) {
		return false;
	}
	--counter;
	return true;
}

bool CppSemaphore::post() {
	std::unique_lock<std::mutex> _lock(mutex);
	++counter;
	conditionVariable.notify_one();
	return true;
}

uint32_t CppSemaphore::getValue() {
	std::unique_lock<std::mutex> _lock(mutex);
	return counter;
}

}
}

#endif /* not defined(__clang__) and not defined(_WIN32) */
