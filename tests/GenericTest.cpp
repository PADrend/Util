/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericTest.h"
#include <cppunit/TestAssert.h>
#include <Util/Generic.h>
#include <array>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
CPPUNIT_TEST_SUITE_REGISTRATION(GenericTest);

static void testContains(const std::array<bool, 7> & expectedResults,
						 const Util::Generic & generic) {
	CPPUNIT_ASSERT_EQUAL(expectedResults[0], generic.contains<bool>());
	CPPUNIT_ASSERT_EQUAL(expectedResults[1], generic.contains<int32_t>());
	CPPUNIT_ASSERT_EQUAL(expectedResults[2], generic.contains<float>());
	CPPUNIT_ASSERT_EQUAL(expectedResults[3], generic.contains<double>());
	CPPUNIT_ASSERT_EQUAL(expectedResults[4], generic.contains<std::string>());
	CPPUNIT_ASSERT_EQUAL(expectedResults[5], generic.contains<std::vector<uint32_t>>());
	CPPUNIT_ASSERT_EQUAL(expectedResults[6], generic.contains<std::nullptr_t>());
}

void GenericTest::testBasic() {
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

	CPPUNIT_ASSERT_EQUAL(true, boolAttr.valid());
	CPPUNIT_ASSERT_EQUAL(true, intAttr.valid());
	CPPUNIT_ASSERT_EQUAL(true, floatAttr.valid());
	CPPUNIT_ASSERT_EQUAL(true, doubleAttr.valid());
	CPPUNIT_ASSERT_EQUAL(true, stringAttr.valid());
	CPPUNIT_ASSERT_EQUAL(true, vecAttr.valid());
	CPPUNIT_ASSERT_EQUAL(false, emptyAttr.valid());

	CPPUNIT_ASSERT_EQUAL(boolValue, *boolAttr.get<bool>());
	CPPUNIT_ASSERT_EQUAL(intValue, *intAttr.get<int32_t>());
	CPPUNIT_ASSERT_EQUAL(floatValue, *floatAttr.get<float>());
	CPPUNIT_ASSERT_EQUAL(doubleValue, *doubleAttr.get<double>());
	CPPUNIT_ASSERT_EQUAL(stringValue, *stringAttr.get<std::string>());
	auto vecPtr = vecAttr.get<std::vector<uint32_t>>();
	CPPUNIT_ASSERT(vecPtr != nullptr);
	CPPUNIT_ASSERT(std::equal(vecValue.cbegin(), vecValue.cend(), vecPtr->cbegin()));

	testContains({{true, false, false, false, false, false, false}}, boolAttr);
	testContains({{false, true, false, false, false, false, false}}, intAttr);
	testContains({{false, false, true, false, false, false, false}}, floatAttr);
	testContains({{false, false, false, true, false, false, false}}, doubleAttr);
	testContains({{false, false, false, false, true, false, false}}, stringAttr);
	testContains({{false, false, false, false, false, true, false}}, vecAttr);
	testContains({{false, false, false, false, false, false, false}}, emptyAttr);

	CPPUNIT_ASSERT_THROW(boolAttr.ref<float>(), std::bad_cast);
	CPPUNIT_ASSERT_THROW(emptyAttr.ref<float>(), std::bad_cast);
}

void GenericTest::testArray() {
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

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), genericVector.size());
	CPPUNIT_ASSERT_EQUAL(boolValue, genericVector[0].ref<bool>());
	CPPUNIT_ASSERT_EQUAL(intValue, genericVector[1].ref<int32_t>());
	CPPUNIT_ASSERT_EQUAL(floatValue, genericVector[2].ref<float>());
	CPPUNIT_ASSERT_EQUAL(doubleValue, genericVector[3].ref<double>());
	CPPUNIT_ASSERT_EQUAL(stringValue, genericVector[4].ref<std::string>());
	CPPUNIT_ASSERT(std::equal(vecValue.cbegin(), vecValue.cend(), genericVector[5].ref<std::vector<uint32_t>>().cbegin()));
}

void GenericTest::testMap() {
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

	CPPUNIT_ASSERT_EQUAL(static_cast<size_t>(6), genericMap.size());
	CPPUNIT_ASSERT_EQUAL(boolValue, *genericMap["bool"].get<bool>());
	CPPUNIT_ASSERT_EQUAL(intValue, *genericMap["int"].get<int32_t>());
	CPPUNIT_ASSERT_EQUAL(floatValue, *genericMap["float"].get<float>());
	CPPUNIT_ASSERT_EQUAL(doubleValue, *genericMap["double"].get<double>());
	CPPUNIT_ASSERT_EQUAL(stringValue, *genericMap["string"].get<std::string>());
	auto vecPtr = genericMap["vec"].get<std::vector<uint32_t>>();
	CPPUNIT_ASSERT(vecPtr != nullptr);
	CPPUNIT_ASSERT(std::equal(vecValue.cbegin(), vecValue.cend(), vecPtr->cbegin()));
}
