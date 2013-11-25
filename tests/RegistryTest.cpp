/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "RegistryTest.h"
#include <cppunit/TestAssert.h>
#include <Util/Registry.h>
#include <algorithm>
#include <cstdint>
#include <functional>
#include <list>
#include <set>
#include <utility>
CPPUNIT_TEST_SUITE_REGISTRATION(RegistryTest);

template<typename container_t,
		 typename element_t>
static bool contains(const container_t & container, const element_t & element) {
	return (std::find(container.cbegin(), container.cend(), element) != container.cend());
}

template<typename registry_t>
static void testRegistry(registry_t & registry) {
	const uint32_t elementA = 5;
	const uint32_t elementB = 17;
	const uint32_t elementC = 42;
	const uint32_t elementD = 128;

	CPPUNIT_ASSERT(registry.getElements().empty());

	auto handleA = registry.registerElement(elementA);

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), registry.getElements().size());
	CPPUNIT_ASSERT(contains(registry.getElements(), elementA));

	auto handleB = registry.registerElement(elementB);

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), registry.getElements().size());
	CPPUNIT_ASSERT(contains(registry.getElements(), elementA));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementB));
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementC));
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementD));

	auto handleC = registry.registerElement(elementC);

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), registry.getElements().size());
	CPPUNIT_ASSERT(contains(registry.getElements(), elementA));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementB));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementC));
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleB));

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), registry.getElements().size());
	CPPUNIT_ASSERT(contains(registry.getElements(), elementA));
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementB));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementC));
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementD));

	auto handleD = registry.registerElement(elementD);

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(3), registry.getElements().size());
	CPPUNIT_ASSERT(contains(registry.getElements(), elementA));
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementB));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementC));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleA));

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), registry.getElements().size());
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementA));
	CPPUNIT_ASSERT(!contains(registry.getElements(), elementB));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementC));
	CPPUNIT_ASSERT(contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleC));

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(1), registry.getElements().size());
	CPPUNIT_ASSERT(contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleD));

	CPPUNIT_ASSERT(registry.getElements().empty());
}

void RegistryTest::testList() {
	Util::Registry<std::list<uint32_t>> listRegistry;
	testRegistry(listRegistry);
}

void RegistryTest::testMultiset() {
	Util::Registry<std::multiset<uint32_t>> multisetRegistry;
	testRegistry(multisetRegistry);
}

void RegistryTest::testFunctions() {
	uint32_t counter = 0;
	auto functionIncr = [&counter] { ++counter; };
	auto functionAddThree = [&counter] { counter += 3; };
	auto functionDouble = [&counter] { counter *= 2; };

	Util::Registry<std::list<std::function<void ()>>> funcRegistry;
	{
		auto handleA = funcRegistry.registerElement(functionDouble);
		auto handleB = funcRegistry.registerElement(functionIncr);
		auto handleC = funcRegistry.registerElement(functionAddThree);
		auto handleD = funcRegistry.registerElement(functionDouble);
		// (((0 * 2) + 1) + 3) * 2 = 8
		for(auto & func : funcRegistry.getElements()) {
			func();
		}
		CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(8), counter);

		funcRegistry.unregisterElement(std::move(handleC));
		// ((8 * 2) + 1) * 2 = 34
		for(auto & func : funcRegistry.getElements()) {
			func();
		}
		CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(34), counter);

		funcRegistry.unregisterElement(std::move(handleA));
		// (34 + 1) * 2 = 70
		for(auto & func : funcRegistry.getElements()) {
			func();
		}
		CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(70), counter);

		funcRegistry.unregisterElement(std::move(handleB));
		// 70 * 2 = 140
		for(auto & func : funcRegistry.getElements()) {
			func();
		}
		CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(140), counter);

		funcRegistry.unregisterElement(std::move(handleD));
		// 140 = 140
		for(auto & func : funcRegistry.getElements()) {
			func();
		}
		CPPUNIT_ASSERT_EQUAL(static_cast<uint32_t>(140), counter);
		CPPUNIT_ASSERT(funcRegistry.getElements().empty());
	}
}

void RegistryTest::testChangesDuringExecution() {
	typedef Util::Registry<std::list<std::function<void ()>>> Registry;
	Registry funcRegistry;
	typedef Registry::handle_t RegistryHandle;

	CPPUNIT_ASSERT(funcRegistry.getElements().empty());

	// Register a function that registers a function when called.
	std::unique_ptr<RegistryHandle> optionalHandleB;
	auto handleA = funcRegistry.registerElement(
		[&funcRegistry, &optionalHandleB] {
			optionalHandleB.reset(new RegistryHandle(funcRegistry.registerElement([] {})));
		}
	);
	// Register a second function that registers a function when called.
	std::unique_ptr<RegistryHandle> optionalHandleD;
	auto handleC = funcRegistry.registerElement(
		[&funcRegistry, &optionalHandleD] {
			optionalHandleD.reset(new RegistryHandle(funcRegistry.registerElement([] {})));
		}
	);

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), funcRegistry.getElements().size());
	CPPUNIT_ASSERT(!optionalHandleB);
	CPPUNIT_ASSERT(!optionalHandleD);

	for(const auto & func : funcRegistry.getElementsCopy()) {
		func();
	}

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), funcRegistry.getElements().size());
	CPPUNIT_ASSERT(optionalHandleB);
	CPPUNIT_ASSERT(optionalHandleD);

	funcRegistry.unregisterElement(std::move(handleC));
	funcRegistry.unregisterElement(std::move(handleA));

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), funcRegistry.getElements().size());
	CPPUNIT_ASSERT(optionalHandleB);
	CPPUNIT_ASSERT(optionalHandleD);

	// Register a function that removes a function when called.
	auto handleE = funcRegistry.registerElement(
		[&funcRegistry, &optionalHandleB] {
			funcRegistry.unregisterElement(std::move(*optionalHandleB.get()));
			optionalHandleB.reset();
		}
	);
	// Register a second function that removes a function when called.
	auto handleF = funcRegistry.registerElement(
		[&funcRegistry, &optionalHandleD] {
			funcRegistry.unregisterElement(std::move(*optionalHandleD.get()));
			optionalHandleD.reset();
		}
	);

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(4), funcRegistry.getElements().size());
	CPPUNIT_ASSERT(optionalHandleB);
	CPPUNIT_ASSERT(optionalHandleD);

	for(const auto & func : funcRegistry.getElementsCopy()) {
		func();
	}

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), funcRegistry.getElements().size());
	CPPUNIT_ASSERT(!optionalHandleB);
	CPPUNIT_ASSERT(!optionalHandleD);

	funcRegistry.unregisterElement(std::move(handleF));
	funcRegistry.unregisterElement(std::move(handleE));

	CPPUNIT_ASSERT(funcRegistry.getElements().empty());

	// Register a function that removes itself from the registry when called.
	optionalHandleB.reset(new RegistryHandle(funcRegistry.registerElement(
		[&funcRegistry, &optionalHandleB] {
			funcRegistry.unregisterElement(std::move(*optionalHandleB.get()));
			optionalHandleB.reset();
		}
	)));
	// Register a second function that removes itself from the registry when called.
	optionalHandleD.reset(new RegistryHandle(funcRegistry.registerElement(
		[&funcRegistry, &optionalHandleD] {
			funcRegistry.unregisterElement(std::move(*optionalHandleD.get()));
			optionalHandleD.reset();
		}
	)));

	CPPUNIT_ASSERT_EQUAL(static_cast<std::size_t>(2), funcRegistry.getElements().size());
	CPPUNIT_ASSERT(optionalHandleB);
	CPPUNIT_ASSERT(optionalHandleD);

	for(const auto & func : funcRegistry.getElementsCopy()) {
		func();
	}

	CPPUNIT_ASSERT(funcRegistry.getElements().empty());
	CPPUNIT_ASSERT(!optionalHandleB);
	CPPUNIT_ASSERT(!optionalHandleD);
}
