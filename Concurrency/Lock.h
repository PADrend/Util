/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_CONCURRENCY_LOCK_H
#define UTIL_CONCURRENCY_LOCK_H

namespace Util {
namespace Concurrency {

/**
 * Class to lock an object on construction and unlock it on destruction.
 *
 * @author Benjamin Eikel
 * @date 2010-10-19
 */
template<class guard_t>
class Lock {
	public:
		//! Constructor locks the object.
		Lock(guard_t & mutualExclusionObject) :
			mutExObject(&mutualExclusionObject) {
			mutExObject->lock();
		}
		//! Destructor unlocks the object.
		~Lock() {
			if(mutExObject != nullptr) {
				mutExObject->unlock();
			}
		}

		/**
		 * Move constructor does not lock the guard and makes sure that the
		 * other lock object does not unlock the guard in its destructor.
		 */
		Lock(Lock && other) : mutExObject(other.mutExObject) {
			other.mutExObject = nullptr;
		}
	private:
		//! Object that is locked and unlocked.
		guard_t * mutExObject;

		Lock(const Lock &) = delete;
		Lock & operator=(const Lock &) = delete;
		Lock & operator=(Lock &&) = delete;
};

}
}

#endif /* UTIL_CONCURRENCY_LOCK_H */
