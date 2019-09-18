/*
	This file is part of the Util library.
	Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2013 Ralf Petring <ralf@petring.net>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch.hpp>
#include "UpdatableHeap.h"
#include <cstddef>
#include <cstdint>
#include <deque>
#include <random>
#include <utility>
#include <vector>
#include <stdexcept>

static std::size_t maxHeapSize = 100000;

TEST_CASE("UpdatableHeapTest_testSequence", "[UpdatableHeapTest]") {
	Util::UpdatableHeap<uint8_t, uint8_t> heap;
	typedef Util::UpdatableHeap<uint8_t, uint8_t>::UpdatableHeapElement * HeapElement_t;
	
	heap.insert(3, 1);
	heap.insert(4, 1);
	heap.insert(1, 1);
	HeapElement_t e1 = heap.insert(0, 1);
	heap.insert(5, 1);
	heap.insert(7, 1);
	heap.insert(2, 1);
	HeapElement_t e2 = heap.insert(6, 1);
	
	heap.erase(e1);
	heap.erase(e2);
	
	uint8_t lastKey = heap.top()->getCost();
	heap.pop();
	while (heap.size() > 0) {
		const uint8_t currentKey = heap.top()->getCost();
		heap.pop();
		REQUIRE(lastKey <= currentKey);
		lastKey = currentKey;
	}
}

TEST_CASE("UpdatableHeapTest_testUpdate", "[UpdatableHeapTest]") {
	Util::UpdatableHeap<uint32_t, uint32_t> heap;
	typedef Util::UpdatableHeap<uint32_t, uint32_t>::UpdatableHeapElement * HeapElement_t;

	std::default_random_engine engine;
	std::uniform_int_distribution<uint32_t> distribution(0, 10000);

	std::vector<HeapElement_t> elementsToUpdate;
	elementsToUpdate.reserve(maxHeapSize);
	for (uint_fast32_t i = 0; i < maxHeapSize; ++i) {
		const uint32_t key = distribution(engine);
		HeapElement_t element = heap.insert(key, i);
		if(std::bernoulli_distribution(1.0 / 3.0)(engine)) {
			elementsToUpdate.push_back(element);
		}
	}
	for (std::vector<HeapElement_t>::const_iterator it = elementsToUpdate.begin(); it != elementsToUpdate.end(); ++it) {
		const uint32_t newKey = distribution(engine);
		heap.update(*it, newKey);
	}
	uint32_t lastKey = heap.top()->getCost();
	heap.pop();
	while (heap.size() > 0) {
		const uint32_t currentKey = heap.top()->getCost();
		heap.pop();
		REQUIRE(lastKey <= currentKey);
		lastKey = currentKey;
	}
}

TEST_CASE("UpdatableHeapTest_testDelete", "[UpdatableHeapTest]") {
	Util::UpdatableHeap<uint32_t, uint32_t> heap;
	typedef Util::UpdatableHeap<uint32_t, uint32_t>::UpdatableHeapElement * HeapElement_t;

	std::default_random_engine engine;
	std::uniform_int_distribution<uint32_t> distribution(0, 10000);

	std::vector<HeapElement_t> elementsToRemove;
	elementsToRemove.reserve(maxHeapSize);
	for (uint_fast32_t i = 0; i < maxHeapSize; ++i) {
		const uint32_t key = distribution(engine);
		HeapElement_t element = heap.insert(key, i);
		if(std::bernoulli_distribution(1.0 / 3.0)(engine)) {
			elementsToRemove.push_back(element);
		}
	}
	for (std::vector<HeapElement_t>::const_iterator it = elementsToRemove.begin(); it != elementsToRemove.end(); ++it) {
		heap.erase(*it);
	}
	uint32_t lastKey = heap.top()->getCost();
	heap.pop();
	while (heap.size() > 0) {
		const uint32_t currentKey = heap.top()->getCost();
		heap.pop();
		REQUIRE(lastKey <= currentKey);
		lastKey = currentKey;
	}
}

TEST_CASE("UpdatableHeapTest_testDeleteUpdate", "[UpdatableHeapTest]") {
	Util::UpdatableHeap<int32_t, std::pair<uint32_t, uint32_t> > heap;
	typedef Util::UpdatableHeap<int32_t, std::pair<uint32_t, uint32_t> >::UpdatableHeapElement * HeapElement_t;

	std::default_random_engine engine;
	std::uniform_int_distribution<uint32_t> distribution(500, 1000);

	std::deque<HeapElement_t> elementsToRemove;
	std::deque<HeapElement_t> elementsToUpdate;
	for (uint_fast32_t i = 0; i < 1000000; ++i) {
		const int32_t key = distribution(engine);
		HeapElement_t element = heap.insert(key, std::make_pair(
														std::uniform_int_distribution<uint32_t>(100, 200)(engine),
														std::uniform_int_distribution<uint32_t>(300, 400)(engine)));
		switch(std::uniform_int_distribution<uint8_t>(0, 1)(engine)) {
			case 0:
				elementsToRemove.push_back(element);
				break;
			case 1:
				elementsToUpdate.push_back(element);
				break;
			default:
				throw std::logic_error("the roof is on fire");
		}
	}
	for (std::deque<HeapElement_t>::const_iterator it = elementsToRemove.begin(); it != elementsToRemove.end(); ++it) {
		heap.erase(*it);
	}
	for (std::deque<HeapElement_t>::const_iterator it = elementsToUpdate.begin(); it != elementsToUpdate.end(); ++it) {
		const int32_t newKey = distribution(engine);
		heap.update(*it, newKey);
	}
	int32_t lastKey = heap.top()->getCost();
	heap.pop();
	while (heap.size() > 0) {
		const int32_t currentKey = heap.top()->getCost();
		const std::pair<float, float> currentData = heap.top()->data;
		heap.pop();
		REQUIRE((currentData.first >= 100 && currentData.first <= 200));
		REQUIRE((currentData.second >= 300 && currentData.second <= 400));
		REQUIRE(lastKey <= currentKey);
		lastKey = currentKey;
	}
}
