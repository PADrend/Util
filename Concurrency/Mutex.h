/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_MUTEX_H
#define UTIL_CONCURRENCY_MUTEX_H

namespace Util {
namespace Concurrency {

/**
 * Class for mutual exclusive execution of code sections.
 *
 * @author Benjamin Eikel
 * @date 2010-06-15
 */
class Mutex {
	public:
		//! Destructor
		virtual ~Mutex() {
		}


		/**
		 * Try to lock the Mutex. If the Mutex is locked by another thread, do not return until it is unlocked by that thread.
		 *
		 * @return @c true if locking was successful, @c false if an error occurred
		 */
		virtual bool lock() = 0;
		/**
		 * Unlock the Mutex. If another thread is waiting for locking this Mutex, activate that thread.
		 *
		 * @return @c true if locking was successful, @c false if an error occurred
		 */
		virtual bool unlock() = 0;

	protected:
		//! Mutex creation is done by a factory.
		Mutex() {
		}

	private:
		Mutex(const Mutex &) = delete;
		Mutex(Mutex &&) = delete;
		Mutex & operator=(const Mutex &) = delete;
		Mutex & operator=(Mutex &&) = delete;
};

}
}

#endif /* UTIL_CONCURRENCY_MUTEX_H */
