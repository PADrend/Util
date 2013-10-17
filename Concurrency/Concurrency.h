/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_CONCURRENCY_H
#define UTIL_CONCURRENCY_CONCURRENCY_H

#include "Lock.h"
#include <type_traits>
#include <utility>

namespace Util {
namespace Concurrency {
class Mutex;
class Semaphore;
class SpinLock;
class Thread;

/**
 * Factory function to create a new Mutex.
 * This will automatically select the preferred implementation of this class.
 *
 * @return Object of preferred Mutex implementation. If no implementation was found, @c nullptr will be returned.
 */
Mutex * createMutex();

/**
 * Factory function to create a new Semaphore.
 * This will automatically select the preferred implementation of this class.
 * The semaphore is initialized with zero.
 *
 * @return Object of preferred Semaphore implementation. If no implementation was found, @c nullptr will be returned.
 */
Semaphore * createSemaphore();

/**
 * Factory function to create a new SpinLock.
 * This will automatically select the preferred implementation of this class.
 *
 * @return Object of preferred SpinLock implementation. If no implementation was found, @c nullptr will be returned.
 */
SpinLock * createSpinLock();

/**
 * Factory function to create a new Thread.
 * This will automatically select the preferred implementation of this class.
 * The thread is not started until the start function of the Thread object is called.
 *
 * @return Object of preferred Semaphore implementation. If no implementation was found, @c nullptr will be returned.
 */
Thread * createThread();

/**
 * Factory function to create a Lock. The function removes the necessity to
 * specify the type of the guard explicitly when creating a lock.
 * 
 * @tparam guard_t Type of guard that the lock will be created for
 * @param guard Guard object that will be given to the lock
 * @return Lock object with an associated guard
 */
template<class guard_t>
inline Lock<typename std::decay<guard_t>::type> createLock(guard_t && guard) {
	return Lock<typename std::decay<guard_t>::type>(std::forward<guard_t>(guard));
}

}
}

#endif /* UTIL_CONCURRENCY_CONCURRENCY_H */
