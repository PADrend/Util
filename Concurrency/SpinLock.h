/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_SPINLOCK_H
#define UTIL_CONCURRENCY_SPINLOCK_H

namespace Util {
namespace Concurrency {

/**
 * Class for mutual exclusive execution of code sections using spin lock objects.
 *
 * @author Benjamin Eikel
 * @date 2010-08-12
 */
class SpinLock {
	public:
		//! Destructor
		virtual ~SpinLock() {
		}


		/**
		 * Try to lock the SpinLock. If the SpinLock is locked by another thread, wait and poll until it is unlocked by that thread.
		 *
		 * @return @c true if locking was successful, @c false if an error occurred
		 */
		virtual bool lock() = 0;
		/**
		 * Unlock the SpinLock.
		 *
		 * @return @c true if locking was successful, @c false if an error occurred
		 */
		virtual bool unlock() = 0;

	protected:
		//! SpinLock creation is done by a factory.
		SpinLock() {
		}

	private:
		SpinLock(const SpinLock &) = delete;
		SpinLock(SpinLock &&) = delete;
		SpinLock & operator=(const SpinLock &) = delete;
		SpinLock & operator=(SpinLock &&) = delete;
};

}
}

#endif /* UTIL_CONCURRENCY_SPINLOCK_H */
