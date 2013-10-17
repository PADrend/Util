/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_SEMAPHORE_H
#define UTIL_CONCURRENCY_SEMAPHORE_H

#include <cstdint>

namespace Util {
namespace Concurrency {

/**
 * Semaphore class for synchronizing of threads.
 *
 * @author Benjamin Eikel
 * @date 2010-06-15
 */
class Semaphore {
	public:
		//! Destructor
		virtual ~Semaphore() {
		}
		//! Call @a wait().
		virtual bool lock() {
			return wait();
		}
		//! Call @a post().
		virtual bool unlock() {
			return post();
		}

		/**
		 * Try to lock the Semaphore. If the value of the Semaphore is positive, return immediately. Otherwise wait until another thread increments it.
		 *
		 * @return @c true if locking was successful, @c false if an error occurred
		 */
		virtual bool wait() = 0;
		/**
		 * Try to lock the Semaphore. Always return immediately and do not wait. If the value of the Semaphore is positive, decrement it.
		 *
		 * @return @c true if locking was successful, @c false if the Semaphore could not be locked.
		 */
		virtual bool tryWait() = 0;
		/**
		 * Unlock the Semaphore. If another thread is waiting for locking this Semaphore, activate that thread.
		 *
		 * @return @c true if locking was successful, @c false if an error occurred
		 */
		virtual bool post() = 0;
		/**
		 * Return the value of the semaphore.
		 */
		virtual uint32_t getValue() = 0;

	protected:
		//! Semaphore creation is done by a factory.
		Semaphore() {
		}

	private:
		Semaphore(const Semaphore &) = delete;
		Semaphore(Semaphore &&) = delete;
		Semaphore & operator=(const Semaphore &) = delete;
		Semaphore & operator=(Semaphore &&) = delete;
};

}
}

#endif /* UTIL_CONCURRENCY_SEMAPHORE_H */
