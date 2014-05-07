/*
	This file is part of the Util library.
	Copyright (C) 2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef SYNCQUEUE_H
#define SYNCQUEUE_H

#include "../Concurrency.h"
#include "../Semaphore.h"
#include "../Mutex.h"
#include "../Lock.h"

#include <memory>
#include <queue>

namespace Util {
namespace Concurrency {

template<typename T>
class SyncQueue {

	private:

		std::unique_ptr<Mutex> mutex;
		std::unique_ptr<Semaphore> semaphore;
		std::queue<T> queue;

	public:

		/**
		 * creates an empty queue
		 */
		SyncQueue() : mutex(Concurrency::createMutex()), semaphore(Concurrency::createSemaphore()) {}

		/**
		 * adds an element to the end of the queue
		 * @param t the element to be added
		 */
		void push(T t) {
			auto lock = createLock(*mutex);
			queue.push(t);
			semaphore->post();
		}

		/**
		 * @return the first element of the queue
		 * @note the element is removed from the queue
		 * @note iff the queue is empty, the calling thread gets blocked
		 */
		T pop() {
			semaphore->wait();
			auto lock = createLock(*mutex);
			T t = queue.front();
			queue.pop();
			return t;
		}

		/**
		 *	@return true iff the queue is empty
		 */
		bool empty() const {
			auto lock = createLock(*mutex);
			return queue.empty();
		}

		/**
		 * @return the current size of the queue
		 */
		size_t size() const {
			auto lock = createLock(*mutex);
			return queue.size();
		}

};

}
}

#endif // SYNCQUEUE_H
