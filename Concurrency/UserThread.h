/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_USERTHREAD_H
#define UTIL_USERTHREAD_H

#include <cstddef>
#include <memory>

namespace Util {
namespace Concurrency {

class Thread;


/*! Extend this class for implementing threads.
	This is a more convenient alternative to manually create a Thread-object using function pointers.	*/
class UserThread{
		std::unique_ptr<Thread> thread;
		bool _running;
	public:
		UserThread(const UserThread & other) = delete;
		UserThread(UserThread && other) = delete;
		
		UserThread();
		virtual ~UserThread();

		/*!	Start the current Thread.
		 *
		 * @return @c true if the Thread was created successfully, @c false if an error occurred
		 */
		bool start();

		/*! Wait for the Thread to end.
		 *	@return @c true if successful, @c false if an error occurred */
		bool join();

		//! Returns true iff the run() method is currently executed.
		bool isRunning()const	{	return	_running;	}

		//! Returns true iff the thread has been started and not yet joined.
		bool isActive()const	{	return	thread!=nullptr;	}

		UserThread & operator=(const UserThread && other) = delete;
	protected:
		/*! ---o
			The main thread method. Should not be called explicitly! */
		virtual void run()=0;

};

}
}

#endif // UTIL_USERTHREAD_H
