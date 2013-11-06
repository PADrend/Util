/*
	This file is part of the Util library.
	Copyright (C) 2007-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2007-2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2007-2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef REFERENCES_H_INCLUDED
#define REFERENCES_H_INCLUDED

#include <utility>

namespace Util {
template<class _T> class WeakPointer;
/**
 * Smart-pointer for objects using reference counters (via base class ReferenceCounter).
 */
template<class _T>
class Reference {
	private:
		_T * obj;
	public:
		typedef Reference<_T> Reference_t;
		Reference() : obj(nullptr) 								{								}
		Reference(_T * _obj) : obj(_obj) 						{	_T::addReference(obj);		}
		Reference(const Reference_t & other) : obj(other.obj)	{	_T::addReference(obj);		}
		Reference(Reference_t && other) : obj(other.obj)		{	other.obj = nullptr;	}
		~Reference()											{	_T::removeReference(obj);	}

		Reference_t & operator=(_T * other) {
			if (obj == other) {
				return *this;
			}
			_T::addReference(other);
			_T::removeReference(obj);
			obj = other;
			return *this;
		}
		Reference_t & operator=(const WeakPointer<_T> & other) {
			if (other.get() == obj) {
				return *this;
			}
			_T::addReference(other.get());
			_T::removeReference(obj);
			obj = other.get();
			return *this;
		}
		Reference_t & operator=(const Reference_t & other) {
			if (this == &other || other.obj == obj) {
				return *this;
			}
			_T::addReference(other.obj);
			_T::removeReference(obj);
			obj = other.obj;
			return *this;
		}
		Reference_t & operator=(Reference_t && other) {
			swap(other);
			other = nullptr;
			return *this;
		}
		void swap(Reference_t & other) {
			using std::swap;
			swap(obj, other.obj);
		}

		/**
		 * Detach the object from the Reference without decreasing
		 * the reference counter or deleting the object.
		 * Set the Reference to nullptr.
		 */
		_T * detach() {
			_T * o = obj;
			obj = nullptr;
			return o;
		}
		/**
		 * Detach the object from the Reference with decreasing
		 * the reference counter and setting the Reference to nullptr.
		 * The object is not deleted if the counter reaches 0.
		 */
		_T * detachAndDecrease() {
			_T * o = obj;
			_T::decreaseReference(o);
			obj = nullptr;
			return o;
		}
		_T * operator->() const									{	return obj;	}
		_T * get() const 										{	return obj;	}

		bool isNull() const										{	return obj == nullptr;	}
		bool isNotNull() const									{	return obj != nullptr;	}

		bool operator==(const Reference_t & other) const		{	return other.obj == obj;	}
		bool operator!=(const Reference_t & other) const		{	return other.obj != obj;	}
		bool operator==(const _T * other) const					{	return other == obj;	}
		bool operator!=(const _T * other) const					{	return other != obj;	}
		bool operator<(const Reference_t& other) const			{	return other.obj < obj;	}
};
/**
 * Weak-smart-pointer.
 */
template<class _T>
class WeakPointer {
	private:
		_T * obj;
	public:
		WeakPointer() : obj(nullptr) 								{	}
		WeakPointer(_T * _obj) : obj(_obj)							{	}
		WeakPointer(const WeakPointer<_T> & other) : obj(other.obj)	{	}
		WeakPointer(WeakPointer<_T> && other) : obj(other.obj)		{	other.obj = nullptr;	}
		~WeakPointer() 												{	}

		WeakPointer<_T> & operator=(const Reference<_T> & other) {
			obj = other.get();
			return *this;
		}
		WeakPointer<_T> & operator=(const WeakPointer<_T> & other) {
			obj = other.obj;
			return *this;
		}
		WeakPointer<_T> & operator=(_T * other) {
			obj = other;
			return *this;
		}
		_T * detach() {
			_T * o = obj;
			obj = nullptr;
			return o;
		}
		void swap(WeakPointer<_T> & other) {
			using std::swap;
			swap(obj, other.obj);
		}
		_T * operator->() const									{	return obj;	}
		_T * get() const 										{	return obj;	}

		bool isNull() const 									{	return obj == nullptr;	}
		bool isNotNull() const									{	return obj != nullptr;	}

		bool operator==(const WeakPointer<_T> & other) const	{	return other.obj == obj;	}
		bool operator!=(const WeakPointer<_T> & other) const	{	return other.obj != obj;	}
		bool operator==(const _T * other) const					{	return other == obj;	}
		bool operator!=(const _T * other) const					{	return other != obj;	}
		bool operator<(const WeakPointer<_T> & other) const		{	return other.obj < obj;	}
};
}

#endif // REFERENCES_H_INCLUDED
