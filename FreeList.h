/*
	This file is part of the Util library.
	Copyright (C) 2020 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UTIL_FREE_LIST_H_
#define UTIL_FREE_LIST_H_

#include "SmallList.h"

#include <cstdlib>
#include <cstring>
#include <cassert>
#include <vector>
#include <variant>

namespace Util {
 
/// Provides an indexed free list with constant-time removals from anywhere
/// in the list without invalidating indices. T must be trivially constructible 
/// and destructible.
// Based on: https://stackoverflow.com/questions/41946007/efficient-and-well-explained-implementation-of-a-quadtree-for-2d-collision-det
template<typename T>
class FreeList {
public:
	class const_iterator;

	/// Creates a new free list.
	FreeList() : first_free(-1) {}
 
	/// Inserts an element to the free list and returns an index to it.
	int32_t insert(const T& element);
 
	/// Inserts an element to the free list and returns an index to it.
	int32_t insert(T&& element);
 
	// Removes the nth element from the free list.
	void erase(int32_t n);
 
	// Removes all elements from the free list.
	void clear() { data.clear(); first_free = -1; }
 
	// Returns the range of valid indices.
	int32_t range() const { return data.size(); }
 
	// Returns the capacity of valid indices.
	int32_t capacity() const { return data.capacity(); }

	// Returns the nth element.
	T& operator[](int32_t n) { return std::get<0>(data[n]); }
 
	// Returns the nth element.
	const T& operator[](int32_t n) const { return std::get<0>(data[n]); }
 
	// Checks if the nth element is valid.
	bool valid(int32_t n) const { return n >= 0 && data.size() > n && std::holds_alternative<T>(data[n]); }
 
	// Reserves space for n elements.
	void reserve(int32_t n) { data.reserve(n); }
 
	// Swaps the contents of the two lists.
	void swap(FreeList& other);

	inline const_iterator begin() const { return const_iterator(this, 0); }
	inline const_iterator end() const { return const_iterator(this, -1); }
 
private:
	using FreeElement = std::variant<T, int32_t>; // element, next
	SmallList<FreeElement> data;
	int32_t first_free;
};
 
//-----------------
 
template <class T>
int32_t FreeList<T>::insert(const T& element) {
	if (first_free != -1) {
		const int32_t index = first_free;
		first_free = std::get<1>(data[first_free]);
		data[index] = element;
		return index;
	}
	else
	{
		FreeElement fe = element;
		data.push_back(fe);
		return data.size() - 1;
	}
}
 
//-----------------
 
template <class T>
int32_t FreeList<T>::insert(T&& element) {
	if (first_free != -1) {
		const int32_t index = first_free;
		first_free = std::get<1>(data[first_free]);
		data[index] = std::move(element);
		return index;
	}
	else
	{
		FreeElement fe = std::move(element);
		data.push_back(fe);
		return data.size() - 1;
	}
}

//-----------------

template <class T>
void FreeList<T>::erase(int32_t n) {
	assert(n >= 0 && n < data.size());
	data[n] = first_free;
	first_free = n;
}

//-----------------

template <class T>
void FreeList<T>::swap(FreeList& other)
{
	const int32_t temp = first_free;
	data.swap(other.data);
	first_free = other.first_free;
	other.first_free = temp;
}

//-----------------
 
template <class T>
class FreeList<T>::const_iterator {
public:
	bool operator==(const const_iterator& other) const {return list == other.list && index == other.index; }
	bool operator!=(const const_iterator& other) const {return !(*this == other);}
	const T& operator*() { return (*list)[index]; }
	inline const_iterator& operator++() {
		if(index >= 0) {
			while(++index < list->range() && !list->valid(index)) { }
			if(index >= list->range()) index = -1;
		}
		return *this;
	}
	
	// iterator traits
	using difference_type = int32_t;
	using value_type = int32_t;
	using pointer = const int32_t*;
	using reference = const int32_t&;
	using iterator_category = std::forward_iterator_tag;
private:
	friend class FreeList<T>;
	const_iterator(const FreeList<T>* _list, int32_t _index) : list(_list), index(_index) {
		if(index >= 0) {
			while(index < list->range() && !list->valid(index)) { ++index; }
			if(index >= list->range()) index = -1;
		}
	}
	const FreeList<T>* list;
	int32_t index;
};

//-----------------

} /* Util */
#endif