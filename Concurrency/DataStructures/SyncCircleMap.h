/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef SYNCCIRCLEMAP_H
#define SYNCCIRCLEMAP_H

#include "../Concurrency.h"
#include "../Mutex.h"
#include "../Semaphore.h"
#include "../Lock.h"

#include <map>
#include <memory>

namespace Util {
namespace Concurrency {

template <typename K, typename V>
class SyncCircleMap {

	private:

		std::map<K, V> map;
		K lastExtracted;
		std::unique_ptr<Mutex> mutex;
		std::unique_ptr<Semaphore> semaphore;

	public:

		/**
		 * creates a new SyncCircleMap
		 */
		SyncCircleMap() : mutex(Concurrency::createMutex()), semaphore(Concurrency::createSemaphore()) {}

		/**
		 * inserts or updates a key/value pair in this datastructure.
		 */
		void set(const K & key, const V & value) {
			Lock lock(mutex.get());
			auto lb = map.lower_bound(key);
			if (lb != map.end() && !(map.key_comp()(key, lb->first))) {
				// Key already in map.
				lb->second = value;
			} else {
				// Insert pair into map.
				map.insert(lb, std::make_pair(key, value));
				semaphore->post();
			}
		}

		/**
		 * @return the first key/value pair in this datastructure with a key
		 * greater than the one returned by the last call to this function.
		 * iff there is no greater key in this datastructure, then the pair
		 * with the smallest key is returned.
		 * 
		 * @note the key that is used for the very first call of this function
		 * is undefined.
		 * 
		 * @note if this datastructure is empty, the calling thread gets blocked.
		 */
		std::pair<K, V> extractNext() {
			semaphore->wait();
			// map can not be empty!
			Lock lock(mutex.get());
			auto actual = map.lower_bound(lastExtracted);
			if(actual == map.end())
				actual = map.begin();
			if(actual->first == lastExtracted) {
				std::advance(actual, 1);
				if(actual == map.end())
					actual = map.begin();
			}
			lastExtracted = actual->first;
			auto ret = *actual;
			map.erase(actual);
			return ret;
		}

		/**
		 * @return true iff this datastructure is empty
		 */
		bool empty() const {
			Lock lock(mutex.get());
			return map.empty();
		}

		/**
		 * @return the number of elements in this datastructure
		 */
		size_t size() const {
			Lock lock(mutex.get());
			return map.size();
		}

};

}
}

#endif // SYNCCIRCLEMAP_H
