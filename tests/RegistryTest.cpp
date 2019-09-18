/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch.hpp>
#include "Registry.h"
#include <algorithm>
#include <cstdint>
#include <functional>
#include <list>
#include <set>
#include <utility>

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

	REQUIRE(registry.getElements().empty());

	auto handleA = registry.registerElement(elementA);

	REQUIRE(static_cast<std::size_t>(1) == registry.getElements().size());
	REQUIRE(contains(registry.getElements(), elementA));

	auto handleB = registry.registerElement(elementB);

	REQUIRE(static_cast<std::size_t>(2) == registry.getElements().size());
	REQUIRE(contains(registry.getElements(), elementA));
	REQUIRE(contains(registry.getElements(), elementB));
	REQUIRE(!contains(registry.getElements(), elementC));
	REQUIRE(!contains(registry.getElements(), elementD));

	auto handleC = registry.registerElement(elementC);

	REQUIRE(static_cast<std::size_t>(3) == registry.getElements().size());
	REQUIRE(contains(registry.getElements(), elementA));
	REQUIRE(contains(registry.getElements(), elementB));
	REQUIRE(contains(registry.getElements(), elementC));
	REQUIRE(!contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleB));

	REQUIRE(static_cast<std::size_t>(2) == registry.getElements().size());
	REQUIRE(contains(registry.getElements(), elementA));
	REQUIRE(!contains(registry.getElements(), elementB));
	REQUIRE(contains(registry.getElements(), elementC));
	REQUIRE(!contains(registry.getElements(), elementD));

	auto handleD = registry.registerElement(elementD);

	REQUIRE(static_cast<std::size_t>(3) == registry.getElements().size());
	REQUIRE(contains(registry.getElements(), elementA));
	REQUIRE(!contains(registry.getElements(), elementB));
	REQUIRE(contains(registry.getElements(), elementC));
	REQUIRE(contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleA));

	REQUIRE(static_cast<std::size_t>(2) == registry.getElements().size());
	REQUIRE(!contains(registry.getElements(), elementA));
	REQUIRE(!contains(registry.getElements(), elementB));
	REQUIRE(contains(registry.getElements(), elementC));
	REQUIRE(contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleC));

	REQUIRE(static_cast<std::size_t>(1) == registry.getElements().size());
	REQUIRE(contains(registry.getElements(), elementD));

	registry.unregisterElement(std::move(handleD));

	REQUIRE(registry.getElements().empty());
}

TEST_CASE("RegistryTest_testList", "[RegistryTest]") {
	Util::Registry<std::list<uint32_t>> listRegistry;
	testRegistry(listRegistry);
}

TEST_CASE("RegistryTest_testMultiset", "[RegistryTest]") {
	Util::Registry<std::multiset<uint32_t>> multisetRegistry;
	testRegistry(multisetRegistry);
}

TEST_CASE("RegistryTest_testFunctions", "[RegistryTest]") {
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
		// (((0 * 2) + 3) + 1) * 2 = 8
		for(const auto & func : funcRegistry.getElements()) {
			func();
		}
		REQUIRE(static_cast<uint32_t>(8) == counter);

		funcRegistry.unregisterElement(std::move(handleC));
		// ((8 * 2) + 1) * 2 = 34
		for(const auto & func : funcRegistry.getElements()) {
			func();
		}
		REQUIRE(static_cast<uint32_t>(34) == counter);

		funcRegistry.unregisterElement(std::move(handleA));
		// (34 * 2) + 1 = 69
		for(const auto & func : funcRegistry.getElements()) {
			func();
		}
		REQUIRE(static_cast<uint32_t>(69) == counter);

		funcRegistry.unregisterElement(std::move(handleB));
		// 69 * 2 = 138
		for(const auto & func : funcRegistry.getElements()) {
			func();
		}
		REQUIRE(static_cast<uint32_t>(138) == counter);

		funcRegistry.unregisterElement(std::move(handleD));
		// 138 = 138
		for(const auto & func : funcRegistry.getElements()) {
			func();
		}
		REQUIRE(static_cast<uint32_t>(138) == counter);
		REQUIRE(funcRegistry.getElements().empty());
	}
}

TEST_CASE("RegistryTest_testChangesDuringExecution", "[RegistryTest]") {
	typedef Util::Registry<std::list<std::function<void ()>>> Registry;
	Registry funcRegistry;
	typedef Registry::handle_t RegistryHandle;

	REQUIRE(funcRegistry.getElements().empty());

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

	REQUIRE(static_cast<std::size_t>(2) == funcRegistry.getElements().size());
	REQUIRE(!optionalHandleB);
	REQUIRE(!optionalHandleD);

	for(const auto & func : funcRegistry.getElementsCopy()) {
		func();
	}

	REQUIRE(static_cast<std::size_t>(4) == funcRegistry.getElements().size());
	REQUIRE(optionalHandleB);
	REQUIRE(optionalHandleD);

	funcRegistry.unregisterElement(std::move(handleC));
	funcRegistry.unregisterElement(std::move(handleA));

	REQUIRE(static_cast<std::size_t>(2) == funcRegistry.getElements().size());
	REQUIRE(optionalHandleB);
	REQUIRE(optionalHandleD);

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

	REQUIRE(static_cast<std::size_t>(4) == funcRegistry.getElements().size());
	REQUIRE(optionalHandleB);
	REQUIRE(optionalHandleD);

	for(const auto & func : funcRegistry.getElementsCopy()) {
		func();
	}

	REQUIRE(static_cast<std::size_t>(2) == funcRegistry.getElements().size());
	REQUIRE(!optionalHandleB);
	REQUIRE(!optionalHandleD);

	funcRegistry.unregisterElement(std::move(handleF));
	funcRegistry.unregisterElement(std::move(handleE));

	REQUIRE(funcRegistry.getElements().empty());

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

	REQUIRE(static_cast<std::size_t>(2) == funcRegistry.getElements().size());
	REQUIRE(optionalHandleB);
	REQUIRE(optionalHandleD);

	for(const auto & func : funcRegistry.getElementsCopy()) {
		func();
	}

	REQUIRE(funcRegistry.getElements().empty());
	REQUIRE(!optionalHandleB);
	REQUIRE(!optionalHandleD);
}
