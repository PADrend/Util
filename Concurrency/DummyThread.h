/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_DUMMYTHREAD_H
#define UTIL_CONCURRENCY_DUMMYTHREAD_H

#include "Thread.h"

namespace Util {
namespace Concurrency {

//! Thread implementation that is an empty dummy.
class DummyThread : public Thread {
	public:
		//! Destructor
		virtual ~DummyThread();

		bool start(void * (*run)(void *), void * data) override;
		bool join() override;

	protected:
		//! Thread creation is done by the factory function @a createThread().
		DummyThread();

		//! Allow access to constructor from factory.
		friend Thread * createThread();
};

}
}

#endif /* UTIL_CONCURRENCY_DUMMYTHREAD_H */
