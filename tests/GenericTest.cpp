/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch.hpp>
#include "Generic.h"
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>

static void testContains(const std::array<bool, 7> & expectedResults,
						 const Util::Generic & generic) {
	REQUIRE(expectedResults[0] == generic.contains<bool>());
	REQUIRE(expectedResults[1] == generic.contains<int32_t>());
	REQUIRE(expectedResults[2] == generic.contains<float>());
	REQUIRE(expectedResults[3] == generic.contains<double>());
	REQUIRE(expectedResults[4] == generic.contains<std::string>());
	REQUIRE(expectedResults[5] == generic.contains<std::vector<uint32_t>>());
	REQUIRE(expectedResults[6] == generic.contains<std::nullptr_t>());
}

TEST_CASE("GenericTest_testBasic", "[GenericTest]") {
	// Test construction from basic types
	const bool boolValue = true;
	Util::Generic boolAttr(boolValue);
	const int32_t intValue = 5;
	Util::Generic intAttr(intValue);
	const float floatValue = 17.0f;
	Util::Generic floatAttr(floatValue);
	const double doubleValue = 42.0;
	Util::Generic doubleAttr(doubleValue);
	Util::Generic emptyAttr;

	const std::string stringValue("String");
	Util::Generic stringAttr(stringValue);

	const std::vector<uint32_t> vecValue{7, 9, 17, 25};
	Util::Generic vecAttr(vecValue);

	REQUIRE(true == boolAttr.valid());
	REQUIRE(true == intAttr.valid());
	REQUIRE(true == floatAttr.valid());
	REQUIRE(true == doubleAttr.valid());
	REQUIRE(true == stringAttr.valid());
	REQUIRE(true == vecAttr.valid());
	REQUIRE(false == emptyAttr.valid());

	REQUIRE(boolValue == *boolAttr.get<bool>());
	REQUIRE(intValue == *intAttr.get<int32_t>());
	REQUIRE(floatValue == *floatAttr.get<float>());
	REQUIRE(doubleValue == *doubleAttr.get<double>());
	REQUIRE(stringValue == *stringAttr.get<std::string>());
	auto vecPtr = vecAttr.get<std::vector<uint32_t>>();
	REQUIRE(vecPtr != nullptr);
	REQUIRE(std::equal(vecValue.cbegin(), vecValue.cend(), vecPtr->cbegin()));

	testContains({{true, false, false, false, false, false, false}}, boolAttr);
	testContains({{false, true, false, false, false, false, false}}, intAttr);
	testContains({{false, false, true, false, false, false, false}}, floatAttr);
	testContains({{false, false, false, true, false, false, false}}, doubleAttr);
	testContains({{false, false, false, false, true, false, false}}, stringAttr);
	testContains({{false, false, false, false, false, true, false}}, vecAttr);
	testContains({{false, false, false, false, false, false, false}}, emptyAttr);

	REQUIRE_THROWS_AS(boolAttr.ref<float>(), std::bad_cast);
	REQUIRE_THROWS_AS(emptyAttr.ref<float>(), std::bad_cast);
}

TEST_CASE("GenericTest_testArray", "[GenericTest]") {
	const bool boolValue = true;
	const int32_t intValue = 5;
	const float floatValue = 17.0f;
	const double doubleValue = 42.0;
	const std::string stringValue("String");
	const std::vector<uint32_t> vecValue{7, 9, 17, 25};

	// Put different types into the same vector
	std::vector<Util::Generic> genericVector;
	genericVector.emplace_back(boolValue);
	genericVector.emplace_back(intValue);
	genericVector.emplace_back(floatValue);
	genericVector.emplace_back(doubleValue);
	genericVector.emplace_back(stringValue);
	genericVector.emplace_back(vecValue);

	REQUIRE(static_cast<size_t>(6) == genericVector.size());
	REQUIRE(boolValue == genericVector[0].ref<bool>());
	REQUIRE(intValue == genericVector[1].ref<int32_t>());
	REQUIRE(floatValue == genericVector[2].ref<float>());
	REQUIRE(doubleValue == genericVector[3].ref<double>());
	REQUIRE(stringValue == genericVector[4].ref<std::string>());
	REQUIRE(std::equal(vecValue.cbegin(), vecValue.cend(), genericVector[5].ref<std::vector<uint32_t>>().cbegin()));
}

TEST_CASE("GenericTest_testMap", "[GenericTest]") {
	const bool boolValue = true;
	const int32_t intValue = 5;
	const float floatValue = 17.0f;
	const double doubleValue = 42.0;
	const std::string stringValue("String");
	const std::vector<uint32_t> vecValue{7, 9, 17, 25};

	// Use different types as values in a map
	std::unordered_map<std::string, Util::Generic> genericMap;
	genericMap["bool"] = boolValue;
	genericMap["int"] = intValue;
	genericMap["float"] = floatValue;
	genericMap["double"] = doubleValue;
	genericMap["string"] = stringValue;
	genericMap["vec"] = vecValue;

	REQUIRE(static_cast<size_t>(6) == genericMap.size());
	REQUIRE(boolValue == *genericMap["bool"].get<bool>());
	REQUIRE(intValue == *genericMap["int"].get<int32_t>());
	REQUIRE(floatValue == *genericMap["float"].get<float>());
	REQUIRE(doubleValue == *genericMap["double"].get<double>());
	REQUIRE(stringValue == *genericMap["string"].get<std::string>());
	auto vecPtr = genericMap["vec"].get<std::vector<uint32_t>>();
	REQUIRE(vecPtr != nullptr);
	REQUIRE(std::equal(vecValue.cbegin(), vecValue.cend(), vecPtr->cbegin()));
}
