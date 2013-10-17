/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "UserThread.h"
#include "Concurrency.h"
#include "Thread.h"
#include "../Macros.h"

namespace Util {
namespace Concurrency {

UserThread::UserThread() : thread(), _running(false) {
}

UserThread::~UserThread() {
	if(thread) {
		// not running? -> efficient join should be possible
		if(!isRunning()){
			join();
			WARN("~UserThread: Destroyed before join().");
		} // still running? -> join() could block, so just issue a warning and see what happens.
		else{
			WARN("~UserThread: Thread still running.");
		}
	}
}

bool UserThread::start() {
	if(thread) {
		WARN("Thread already running.");
		return false;
	}
	thread.reset(createThread());

	struct fnWrapper{
		static void * run(void * userData) {
			UserThread * userThread = static_cast<UserThread*>(userData);
			userThread->_running = true;
			userThread->run();
			userThread->_running = false;
			return nullptr;
		}
	};
	bool success = thread->start(fnWrapper::run, this);
	if(!success) {
		thread.reset();
	}
	return success;
}

bool UserThread::join() {
	if(!thread) {
		WARN("Thread has not been started.");
		return false;
	}
	bool success = thread->join();

	thread.reset();
	_running = false; // just to make sure

	return success;
}

}
}
