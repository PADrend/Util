/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_THREAD_H
#define UTIL_CONCURRENCY_THREAD_H

namespace Util {
namespace Concurrency {

/**
 * Class representing a concurrent thread of execution.
 *
 * @author Benjamin Eikel
 * @date 2010-06-15
 */
class Thread {
	public:
		//! Destructor terminates the Thread
		virtual ~Thread() {
		}


		/**
		 * Start the current Thread. The given function is called with the given data argument.
		 *
		 * @param run Function, that will be executed by the Thread
		 * @param data Argument, that will be passed to the function
		 * @return @c true if the Thread was created successfully, @c false if an error occurred
		 */
		virtual bool start(void * (*run)(void *), void * data) = 0;
		/**
		 * Wait for the Thread to end.
		 *
		 * @return @c true if successful, @c false if an error occurred
		 */
		virtual bool join() = 0;

	protected:
		//! Thread creation is done by a factory.
		Thread() {
		}

	private:
		Thread(const Thread &) = delete;
		Thread(Thread &&) = delete;
		Thread & operator=(const Thread &) = delete;
		Thread & operator=(Thread &&) = delete;
};

}
}

#endif /* UTIL_CONCURRENCY_THREAD_H */
