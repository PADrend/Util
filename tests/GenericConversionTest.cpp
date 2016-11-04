/*
	This file is part of the Util library.
	Copyright (C) 2013-2015 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericConversionTest.h"
#include <cppunit/TestAssert.h>
#include "Generic.h"
#include "GenericConversion.h"
#include "StringIdentifier.h"
#include "StringUtils.h"
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>
CPPUNIT_TEST_SUITE_REGISTRATION(GenericConversionTest);

typedef std::vector<Util::Generic> GenericArray;
typedef std::unordered_map<Util::StringIdentifier, Util::Generic> GenericMap;

static bool checkGenericArraysEqual(const GenericArray & expected, const GenericArray & actual);
static bool checkGenericMapsEqual(const GenericMap & expected, const GenericMap & actual);

static bool checkGenericsEqual(const Util::Generic & expected, const Util::Generic & actual) {
	CPPUNIT_ASSERT(expected.valid());
	CPPUNIT_ASSERT(actual.valid());
	if(expected.contains<bool>()) {
		CPPUNIT_ASSERT(actual.contains<bool>());
		CPPUNIT_ASSERT_EQUAL(expected.ref<bool>(), actual.ref<bool>());
		return true;
	} else if(expected.contains<float>()) {
		CPPUNIT_ASSERT(actual.contains<float>());
		CPPUNIT_ASSERT_EQUAL(expected.ref<float>(), actual.ref<float>());
		return true;
	} else if(expected.contains<std::string>()) {
		CPPUNIT_ASSERT(actual.contains<std::string>());
		CPPUNIT_ASSERT_EQUAL(expected.ref<std::string>(), actual.ref<std::string>());
		return true;
	} else if(expected.contains<GenericArray>()) {
		CPPUNIT_ASSERT(actual.contains<GenericArray>());
		return checkGenericArraysEqual(expected.ref<GenericArray>(), actual.ref<GenericArray>());
	} else if(expected.contains<GenericMap>()) {
		CPPUNIT_ASSERT(actual.contains<GenericMap>());
		return checkGenericMapsEqual(expected.ref<GenericMap>(), actual.ref<GenericMap>());
	}
	return false;
}

static bool checkGenericArraysEqual(const GenericArray & expected, const GenericArray & actual) {
	CPPUNIT_ASSERT_EQUAL(expected.size(), actual.size());
	return std::equal(expected.cbegin(), expected.cend(), actual.cbegin(), &checkGenericsEqual);
}

static bool checkGenericMapsEqual(const GenericMap & expected, const GenericMap & actual) {
	CPPUNIT_ASSERT_EQUAL(expected.size(), actual.size());
	for(auto expectedElement : expected) {
		CPPUNIT_ASSERT(actual.count(expectedElement.first) != 0);
		if(!checkGenericsEqual(expectedElement.second, actual.at(expectedElement.first))) {
			return false;
		}
	}
	return true;
}

template<typename WriteValueType, typename ReadValueType>
static void testGenericSerialization(const WriteValueType writeValue,
									 const std::string & expectedSerialization,
									 const ReadValueType readValue) {
	const Util::Generic genericExport(writeValue);
	std::stringstream tempStream;
	Util::GenericConversion::toJSON(genericExport, tempStream);
	const std::string actualSerialization = tempStream.str();
	CPPUNIT_ASSERT_EQUAL(expectedSerialization, actualSerialization);

	const Util::Generic genericImport = Util::GenericConversion::fromJSON(tempStream);
	CPPUNIT_ASSERT_EQUAL(readValue, genericImport.ref<ReadValueType>());
}

static void testGenericStringSerialization(const std::string & str) {
	testGenericSerialization<std::string, std::string>(str,
													   std::string("\"") + Util::StringUtils::escape(str) + std::string("\""),
													   str);
}

void GenericConversionTest::testBasicSerialization() {
	testGenericSerialization<bool, bool>(true, "true", true);
	testGenericSerialization<bool, bool>(false, "false", false);

	testGenericSerialization<double, float>(25.6789, "25.6789", 25.6789f);
	testGenericSerialization<float, float>(12.345f, "12.345", 12.345f);
	testGenericSerialization<long, float>(-234978l, "-234978", -234978.0f);
	testGenericSerialization<unsigned long, float>(413214ul, "413214", 413214.0f);
	testGenericSerialization<int, float>(-234978, "-234978", -234978.0f);
	testGenericSerialization<unsigned int, float>(413214u, "413214", 413214.0f);
	testGenericSerialization<short, float>(-1200, "-1200", -1200.0f);
	testGenericSerialization<unsigned short, float>(837u, "837", 837.0f);

	testGenericSerialization<char, float>(-128, "-128", -128.0f);
	testGenericSerialization<char, float>(0, "0", 0.0f);
	testGenericSerialization<char, float>(127, "127", 127.0f);

	testGenericSerialization<unsigned char, float>(0u, "0", 0.0f);
	testGenericSerialization<unsigned char, float>(127u, "127", 127.0f);
	testGenericSerialization<unsigned char, float>(255u, "255", 255.0f);

	testGenericStringSerialization("Hello, world!");
	testGenericStringSerialization("abc");
	testGenericStringSerialization("ABC");
	testGenericStringSerialization("123");
	testGenericStringSerialization("x");
	testGenericStringSerialization("");
	testGenericStringSerialization("\"\"");
	testGenericStringSerialization("''");
	testGenericStringSerialization("'\"");
	testGenericStringSerialization("[1, 2, 3]");
	testGenericStringSerialization("a\nb\nc");
}

void GenericConversionTest::testArraySerialization() {
	GenericArray array;
	array.emplace_back(true);
	array.emplace_back(std::string("[1, 2, \"xyz\"]"));
	array.emplace_back(12345.6f);
	array.emplace_back(false);

	GenericArray innerArray;
	innerArray.emplace_back(std::string("one"));
	innerArray.emplace_back(std::string("two"));
	innerArray.emplace_back(std::string("three"));
	array.emplace_back(innerArray);

	const Util::Generic genericArray(array);

	std::stringstream tempStream;
	Util::GenericConversion::toJSON(genericArray, tempStream);
	const std::string serialization = tempStream.str();

	const Util::Generic importedGenericArray = Util::GenericConversion::fromJSON(tempStream);
	CPPUNIT_ASSERT(checkGenericsEqual(genericArray, importedGenericArray));
}

void GenericConversionTest::testMapSerialization() {
	GenericMap map;
	map.emplace(Util::StringIdentifier("firstBool"), Util::Generic(true));
	map.emplace(Util::StringIdentifier("secondBool"), Util::Generic(false));
	map.emplace(Util::StringIdentifier("first string"), Util::Generic(std::string("[1, 2, \"xyz\"]")));
	map.emplace(Util::StringIdentifier("second string"), Util::Generic(std::string("hello")));
	map.emplace(Util::StringIdentifier("first number"), Util::Generic(12345.6f));
	map.emplace(Util::StringIdentifier("second number"), Util::Generic(-4321.1f));

	GenericMap innerMap;
	innerMap.emplace(Util::StringIdentifier("a"), Util::Generic(std::string("one")));
	innerMap.emplace(Util::StringIdentifier("b"), Util::Generic(std::string("two")));
	innerMap.emplace(Util::StringIdentifier("c"), Util::Generic(std::string("three")));
	map.emplace(Util::StringIdentifier("innerMap"), Util::Generic(innerMap));

	const Util::Generic genericMap(map);

	std::stringstream tempStream;
	Util::GenericConversion::toJSON(genericMap, tempStream);
	const std::string serialization = tempStream.str();

	const Util::Generic importedGenericArray = Util::GenericConversion::fromJSON(tempStream);
	CPPUNIT_ASSERT(checkGenericsEqual(genericMap, importedGenericArray));
}
