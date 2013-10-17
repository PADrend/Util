/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef REFERENCECOUNTER_H_INCLUDED
#define REFERENCECOUNTER_H_INCLUDED

#include "References.h"

namespace Util {

template<class Obj_t>
struct ObjectDeleter {
	static void release(Obj_t * obj) {
		delete obj;
	}
};

/**
 * @brief Base class for reference-counted classes
 * 
 * Base class that can be used to create reference-counted classes.
 * Let a class inherit ReferenceCounter to make that class reference-counted.
 * It is non-virtual to be usable from subclasses that are non-virtual as well.
 * 
 * @tparam Obj_t The inheriting class
 * @tparam ObjReleaseHandler_t A class with a function <tt>static void release(Obj_t *)</tt>
 */
template<class Obj_t, class ObjReleaseHandler_t = ObjectDeleter<Obj_t>>
class ReferenceCounter {
	private:
		int refCounter;

	protected:
		//! Type definition for this class. It can be used to call a parent's constructor from the inheriting class.
		typedef ReferenceCounter<Obj_t, ObjReleaseHandler_t> ReferenceCounter_t;

	public:
		//! Type definition for a reference to an object of the inheriting class.
		typedef Reference<Obj_t> ref_t;

		//! Default constructor
		ReferenceCounter() : refCounter(0) {
		}

		//! Copy constructor (counter of the new object must be zero)
		ReferenceCounter(const ReferenceCounter &) : refCounter(0) {
		}

		//! Forbid move constructor
		ReferenceCounter(ReferenceCounter &&) = delete;

		//! Forbid copy assignment
		ReferenceCounter & operator=(const ReferenceCounter &) = delete;

		//! Forbid move assignment
		ReferenceCounter & operator=(ReferenceCounter &&) = delete;

		//! Return the current number of references to this object.
		int countReferences() const {
			return refCounter;
		}

		//! Increase the reference counter of object @p o.
		static void addReference(Obj_t * o) {
			if(o != nullptr) {
				++o->refCounter;
			}
		}

		//! Decrease the reference counter of object @p o. If the counter is zero, the object is released.
		static void removeReference(Obj_t * o) {
			if(o != nullptr && (--o->refCounter) == 0) {
				ObjReleaseHandler_t::release(o);
			}
		}

		//! Decrease the reference counter of object @p o. The object is not released, even if the counter is zero.
		static void decreaseReference(Obj_t * o) {
			if(o != nullptr) {
				--o->refCounter;
			}
		}
};

}

#endif // REFERENCECOUNTER_H_INCLUDED
