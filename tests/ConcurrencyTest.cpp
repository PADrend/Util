/*
	This file is part of the Util library.
	Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "ConcurrencyTest.h"
#include <Util/Concurrency/Concurrency.h>
#include <Util/Concurrency/Mutex.h>
#include <Util/Concurrency/Semaphore.h>
#include <Util/Concurrency/Thread.h>
#include <Util/Macros.h>
#include <cppunit/TestAssert.h>
#include <cstdint>
#include <cstdlib>
#include <memory>
CPPUNIT_TEST_SUITE_REGISTRATION(ConcurrencyTest);

using namespace Util::Concurrency;
using namespace CppUnit;

struct ThreadData_t {
	std::unique_ptr<Mutex> mutex;
	std::unique_ptr<Semaphore> sem;
	uint32_t counter;
	uint32_t barrier;
};

static void * threadAFun(void * arg) {
	// Section 1: Semaphore test
	ThreadData_t * data = reinterpret_cast<ThreadData_t *> (arg);
	for (uint_fast16_t i = 0; i < 5000; ++i) {
		// Consume
		FAIL_IF(!data->sem->wait());
	}

	FAIL_IF(!data->mutex->lock());
	++data->barrier;
	FAIL_IF(!data->mutex->unlock());
	while (true) {
		// Wait until all three threads have finished their first section.
		FAIL_IF(!data->mutex->lock());
		if (data->barrier == 3) {
			FAIL_IF(!data->mutex->unlock());
			break;
		}
		FAIL_IF(!data->mutex->unlock());
	}

	// Section 2: Mutex test
	for (uint_fast32_t i = 0; i < 10000000; ++i) {
		FAIL_IF(!data->mutex->lock());
		++data->counter;
		FAIL_IF(!data->mutex->unlock());
	}
	return nullptr;
}

static void * threadBFun(void * arg) {
	// Section 1: Semaphore test
	ThreadData_t * data = reinterpret_cast<ThreadData_t *> (arg);
	for (uint_fast16_t i = 0; i < 5000; ++i) {
		// Consume
		FAIL_IF(!data->sem->wait());
	}

	FAIL_IF(!data->mutex->lock());
	++data->barrier;
	FAIL_IF(!data->mutex->unlock());
	while (true) {
		// Wait until all three threads have finished their first section.
		FAIL_IF(!data->mutex->lock());
		if (data->barrier == 3) {
			FAIL_IF(!data->mutex->unlock());
			break;
		}
		FAIL_IF(!data->mutex->unlock());
	}

	// Section 2: Mutex test
	for (uint_fast32_t i = 0; i < 10000000; ++i) {
		FAIL_IF(!data->mutex->lock());
		++data->counter;
		FAIL_IF(!data->mutex->unlock());
	}
	return nullptr;
}

static void * threadCFun(void * arg) {
	// Section 1: Semaphore test
	ThreadData_t * data = reinterpret_cast<ThreadData_t *> (arg);
	for (uint_fast16_t i = 0; i < 10123; ++i) {
		// Produce
		FAIL_IF(!data->sem->post());
	}

	FAIL_IF(!data->mutex->lock());
	++data->barrier;
	FAIL_IF(!data->mutex->unlock());
	while (true) {
		// Wait until all three threads have finished their first section.
		FAIL_IF(!data->mutex->lock());
		if (data->barrier == 3) {
			FAIL_IF(!data->mutex->unlock());
			break;
		}
		FAIL_IF(!data->mutex->unlock());
	}

	// Section 2: Mutex test
	for (uint_fast32_t i = 0; i < 10000000; ++i) {
		FAIL_IF(!data->mutex->lock());
		++data->counter;
		FAIL_IF(!data->mutex->unlock());
	}
	return nullptr;
}

void ConcurrencyTest::test() {
	std::unique_ptr<ThreadData_t> data(new ThreadData_t);
	data->mutex.reset(createMutex());
	data->sem.reset(createSemaphore());
	data->counter = 0;
	data->barrier = 0;

	std::unique_ptr<Thread> threadA(createThread());
	std::unique_ptr<Thread> threadB(createThread());
	std::unique_ptr<Thread> threadC(createThread());

	threadA->start(threadAFun, data.get());
	threadB->start(threadBFun, data.get());
	threadC->start(threadCFun, data.get());

	threadA->join();
	threadB->join();
	threadC->join();

	CPPUNIT_ASSERT_EQUAL(data->sem->getValue(), 123u);
	CPPUNIT_ASSERT_EQUAL(data->counter, 30000000u);

	threadA.reset();
	threadB.reset();
	threadC.reset();
}

