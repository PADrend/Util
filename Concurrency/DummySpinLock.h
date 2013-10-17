/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_DUMMYSPINLOCK_H
#define UTIL_CONCURRENCY_DUMMYSPINLOCK_H

#include "SpinLock.h"

namespace Util {
namespace Concurrency {

//! SpinLock implementation that is an empty dummy.
class DummySpinLock : public SpinLock {
	public:
		//! Destructor
		virtual ~DummySpinLock();

		bool lock() override;
		bool unlock() override;

	protected:
		//! SpinLock creation is done by the factory method @a createSpinLock().
		DummySpinLock();

		//! Allow access to constructor from factory.
		friend SpinLock * createSpinLock();
};

}
}

#endif /* UTIL_CONCURRENCY_DUMMYSPINLOCK_H */
