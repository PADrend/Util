/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_DUMMYSEMAPHORE_H
#define UTIL_CONCURRENCY_DUMMYSEMAPHORE_H

#include "Semaphore.h"

namespace Util {
namespace Concurrency {

//! Semaphore implementation that is an empty dummy.
class DummySemaphore : public Semaphore {
	public:
		//! Destructor
		virtual ~DummySemaphore();

		bool wait() override;
		bool tryWait() override;
		bool post() override;
		uint32_t getValue() override;

	protected:
		//! Semaphore creation is done by the factory method @a createSemaphore().
		DummySemaphore();

		//! Allow access to constructor from factory.
		friend Semaphore * createSemaphore();
};

}
}

#endif /* UTIL_CONCURRENCY_DUMMYSEMAPHORE_H */
