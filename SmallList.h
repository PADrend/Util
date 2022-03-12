/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/

#ifndef UTIL_SMALL_LIST_H_
#define UTIL_SMALL_LIST_H_
 
#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>

namespace Util {

// Stores a random-access sequence of elements similar to vector, but avoids 
// heap allocations for small lists. T must be trivially constructible and 
// destructible.
// Based on: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
template <class T>
class SmallList {
public:
	// Creates an empty list.
	SmallList() = default;
 
	// Creates a copy of the specified list.
	SmallList(const SmallList& other);
 
	// Copies the specified list.
	SmallList& operator=(const SmallList& other) { SmallList(other).swap(*this); return *this; }
 
	// Destroys the list.
	~SmallList() { if(ld.data != ld.buf) free(ld.data); }
 
	// Returns the number of elements in the list.
	int32_t size() const { return ld.num; }
 
	// Returns the capacity of elements in the list.
	int32_t capacity() const { return ld.cap; }
 
	// Returns the nth element.
	T& operator[](int32_t n) { assert(n >= 0 && n < ld.num); return ld.data[n]; }
 
	// Returns the nth element in the list.
	const T& operator[](int32_t n) const { assert(n >= 0 && n < ld.num); return ld.data[n]; }
 
	// Returns an index to a matching element in the list or -1
	// if the element is not found.
	int32_t find_index(const T& element) const;
 
	// Clears the list.
	void clear() { ld.num = 0; }
 
	// Reserves space for n elements.
	void reserve(int32_t n);
 
	// Inserts an element to the back of the list.
	void push_back(const T& element);
 
	/// Pops an element off the back of the list.
	T pop_back() { return ld.data[--ld.num]; }
 
	// Swaps the contents of this list with the other.
	void swap(SmallList& other);
 
	// Returns a pointer to the underlying buffer.
	T* data() { return ld.data; }
 
	// Returns a pointer to the underlying buffer.
	const T* data() const { return ld.data; }
 
private:
	enum {fixed_cap = 256};
	struct ListData
	{
		ListData() : data(buf), num(0), cap(fixed_cap) {}
		T buf[fixed_cap];
		T* data;
		int32_t num;
		int32_t cap;
	};
	ListData ld;
};

//-----------------
 
template <class T>
SmallList<T>::SmallList(const SmallList& other) {
	if (other.ld.cap == fixed_cap) {
		ld = other.ld;
		ld.data = ld.buf;
	} else {
		reserve(other.ld.num);
		for (int32_t j=0; j < other.size(); ++j)
			ld.data[j] = other.ld.data[j];
		ld.num = other.ld.num;
		ld.cap = other.ld.cap;
	}
}

//-----------------
 
template <class T>
int32_t SmallList<T>::find_index(const T& element) const {
	for (int32_t j=0; j < ld.num; ++j) {
		if (ld.data[j] == element)
			return j;
	}
	return -1;
}

//-----------------
 
template <class T>
void SmallList<T>::reserve(int32_t n) {
	enum {type_size = sizeof(T)};
	if (n > ld.cap) {
		if (ld.cap == fixed_cap) {
			ld.data = static_cast<T*>(malloc(n * type_size));
			memcpy(ld.data, ld.buf, sizeof(ld.buf));
		} else {
			ld.data = static_cast<T*>(realloc(ld.data, n * type_size));
		}
		ld.cap = n;
	}
}

//-----------------
 
template <class T>
void SmallList<T>::push_back(const T& element) {
	if (ld.num >= ld.cap)
		reserve(ld.cap * 2);
	ld.data[ld.num++] = element;
}

//-----------------
 
template <class T>
void SmallList<T>::swap(SmallList& other) {
	ListData& ld1 = ld;
	ListData& ld2 = other.ld;
 
	const int32_t use_fixed1 = ld1.data == ld1.buf;
	const int32_t use_fixed2 = ld2.data == ld2.buf;
 
	const ListData temp = ld1;
	ld1 = ld2;
	ld2 = temp;
 
	if (use_fixed1)
		ld2.data = ld2.buf;
	if (use_fixed2)
		ld1.data = ld1.buf;
}

//-----------------
 
} /* Util */
#endif