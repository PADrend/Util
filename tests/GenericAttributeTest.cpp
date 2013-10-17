/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericAttributeTest.h"
#include <cppunit/TestAssert.h>
#include <Util/GenericAttribute.h>
#include <cstdint>
#include <memory>
#include <string>
CPPUNIT_TEST_SUITE_REGISTRATION(GenericAttributeTest);

static void testBoolGetters(const Util::BoolAttribute & attribute, bool expectedValue, const std::string & expectedString) {
	CPPUNIT_ASSERT_EQUAL(expectedValue, attribute.get());
	CPPUNIT_ASSERT_EQUAL(expectedValue, attribute.toBool());
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(expectedValue), attribute.toInt());
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(expectedValue), attribute.toUnsignedInt());
	CPPUNIT_ASSERT_EQUAL(static_cast<float>(expectedValue), attribute.toFloat());
	CPPUNIT_ASSERT_EQUAL(static_cast<double>(expectedValue), attribute.toDouble());
	CPPUNIT_ASSERT_EQUAL(expectedString, attribute.toString());
}

void GenericAttributeTest::testBool() {
	const bool value = true;
	{ // Direct construction
		Util::BoolAttribute boolAttribute(value);
		testBoolGetters(boolAttribute, value, "true");

		const bool secondValue = false;
		boolAttribute.set(secondValue);
		testBoolGetters(boolAttribute, secondValue, "false");

		const bool thirdValue = true;
		boolAttribute = Util::BoolAttribute(thirdValue);
		testBoolGetters(boolAttribute, thirdValue, "true");
	}
	{ // Specific factory
		std::unique_ptr<Util::BoolAttribute> boolAttribute(Util::GenericAttribute::createBool(value));
		testBoolGetters(*boolAttribute, value, "true");

		const bool valueXor = value ^ value;
		boolAttribute->set(valueXor);
		testBoolGetters(*boolAttribute, valueXor, "false");

		std::unique_ptr<Util::BoolAttribute> clonedBoolAttribute(boolAttribute->clone());
		CPPUNIT_ASSERT(*boolAttribute == *clonedBoolAttribute);
		boolAttribute.reset();

		testBoolGetters(*clonedBoolAttribute, valueXor, "false");
	}
	{ // Generic factory
		std::unique_ptr<Util::GenericAttribute> genericBoolAttribute(Util::GenericAttribute::create(value));
		CPPUNIT_ASSERT(genericBoolAttribute->isA<Util::BoolAttribute>());
		CPPUNIT_ASSERT_EQUAL(value, genericBoolAttribute->toType<Util::BoolAttribute>()->get());
		CPPUNIT_ASSERT_EQUAL(true, genericBoolAttribute->toBool());
		CPPUNIT_ASSERT_EQUAL(static_cast<int>(value), genericBoolAttribute->toInt());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(value), genericBoolAttribute->toUnsignedInt());
		CPPUNIT_ASSERT_EQUAL(static_cast<float>(value), genericBoolAttribute->toFloat());
		CPPUNIT_ASSERT_EQUAL(static_cast<double>(value), genericBoolAttribute->toDouble());
		CPPUNIT_ASSERT_EQUAL(std::string("true"), genericBoolAttribute->toString());
	}
}

template<typename attribute_t, typename val_t>
static void testNumberGetters(const attribute_t & attribute, val_t expectedValue, const std::string & expectedString) {
	CPPUNIT_ASSERT_EQUAL(expectedValue, attribute.get());
	CPPUNIT_ASSERT_EQUAL(true, attribute.toBool());
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(expectedValue), attribute.toInt());
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(expectedValue), attribute.toUnsignedInt());
	CPPUNIT_ASSERT_EQUAL(static_cast<float>(expectedValue), attribute.toFloat());
	CPPUNIT_ASSERT_EQUAL(static_cast<double>(expectedValue), attribute.toDouble());
	CPPUNIT_ASSERT_EQUAL(expectedString, attribute.toString());
}

template<typename val_t>
static void typedNumberTest() {
	const val_t value = 5;
	{ // Direct construction
		Util::_NumberAttribute<val_t> numberAttribute(value);
		testNumberGetters(numberAttribute, value, "5");

		const val_t secondValue = 7;
		numberAttribute.set(secondValue);
		testNumberGetters(numberAttribute, secondValue, "7");

		const val_t thirdValue = 42;
		numberAttribute = Util::_NumberAttribute<val_t>(thirdValue);
		testNumberGetters(numberAttribute, thirdValue, "42");
	}
	{ // Specific factory
		std::unique_ptr<Util::_NumberAttribute<val_t>> numberAttribute(Util::GenericAttribute::createNumber<val_t>(value));
		testNumberGetters(*numberAttribute, value, "5");

		const val_t valueSquared = value * value;
		numberAttribute->set(valueSquared);
		testNumberGetters(*numberAttribute, valueSquared, "25");

		std::unique_ptr<Util::_NumberAttribute<val_t>> clonedNumberAttribute(numberAttribute->clone());
		CPPUNIT_ASSERT(*numberAttribute == *clonedNumberAttribute);
		numberAttribute.reset();

		testNumberGetters(*clonedNumberAttribute, valueSquared, "25");
	}
	{ // Generic factory
		std::unique_ptr<Util::GenericAttribute> genericNumberAttribute(Util::GenericAttribute::create(value));
		CPPUNIT_ASSERT(genericNumberAttribute->isA<Util::_NumberAttribute<val_t>>());
		CPPUNIT_ASSERT_EQUAL(value, genericNumberAttribute->toType<Util::_NumberAttribute<val_t>>()->get());
		CPPUNIT_ASSERT_EQUAL(true, genericNumberAttribute->toBool());
		CPPUNIT_ASSERT_EQUAL(static_cast<int>(value), genericNumberAttribute->toInt());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(value), genericNumberAttribute->toUnsignedInt());
		CPPUNIT_ASSERT_EQUAL(static_cast<float>(value), genericNumberAttribute->toFloat());
		CPPUNIT_ASSERT_EQUAL(std::string("5"), genericNumberAttribute->toString());
	}
}

void GenericAttributeTest::testNumber() {
	typedNumberTest<char>();
	typedNumberTest<int8_t>();
	typedNumberTest<uint8_t>();
	typedNumberTest<int16_t>();
	typedNumberTest<uint16_t>();
	typedNumberTest<int32_t>();
	typedNumberTest<uint32_t>();
	typedNumberTest<int64_t>();
	typedNumberTest<uint64_t>();
	typedNumberTest<float>();
	typedNumberTest<double>();
}

static void testStringGetters(const Util::StringAttribute & attribute, const std::string & expectedString, bool expectedBool, int expectedNumber) {
	CPPUNIT_ASSERT_EQUAL(expectedString, attribute.get());
	CPPUNIT_ASSERT_EQUAL(expectedBool, attribute.toBool());
	CPPUNIT_ASSERT_EQUAL(static_cast<int>(expectedNumber), attribute.toInt());
	CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(expectedNumber), attribute.toUnsignedInt());
	CPPUNIT_ASSERT_EQUAL(static_cast<float>(expectedNumber), attribute.toFloat());
	CPPUNIT_ASSERT_EQUAL(static_cast<double>(expectedNumber), attribute.toDouble());
	CPPUNIT_ASSERT_EQUAL(expectedString, attribute.toString());
}

void GenericAttributeTest::testString() {
	const std::string value = "abc";
	{ // Direct construction
		Util::StringAttribute stringAttribute(value);
		testStringGetters(stringAttribute, value, false, 0);

		const std::string secondValue = "28";
		stringAttribute.set(secondValue);
		testStringGetters(stringAttribute, secondValue, false, 28);

		const std::string thirdValue = "true";
		stringAttribute = Util::StringAttribute(thirdValue);
		testStringGetters(stringAttribute, thirdValue, true, 0);
	}
	{ // Specific factory
		std::unique_ptr<Util::StringAttribute> stringAttribute(Util::GenericAttribute::createString(value));
		testStringGetters(*stringAttribute, value, false, 0);

		const std::string valueDouble = value + value;
		stringAttribute->set(valueDouble);
		testStringGetters(*stringAttribute, valueDouble, false, 0);

		std::unique_ptr<Util::StringAttribute> clonedStringAttribute(stringAttribute->clone());
		CPPUNIT_ASSERT(*stringAttribute == *clonedStringAttribute);
		stringAttribute.reset();

		testStringGetters(*clonedStringAttribute, valueDouble, false, 0);
	}
	{ // Generic factory
		std::unique_ptr<Util::GenericAttribute> genericStringAttribute(Util::GenericAttribute::create(value));
		CPPUNIT_ASSERT(genericStringAttribute->isA<Util::StringAttribute>());
		CPPUNIT_ASSERT_EQUAL(value, genericStringAttribute->toType<Util::StringAttribute>()->get());
		CPPUNIT_ASSERT_EQUAL(false, genericStringAttribute->toBool());
		CPPUNIT_ASSERT_EQUAL(static_cast<int>(0), genericStringAttribute->toInt());
		CPPUNIT_ASSERT_EQUAL(static_cast<unsigned int>(0), genericStringAttribute->toUnsignedInt());
		CPPUNIT_ASSERT_EQUAL(static_cast<float>(0), genericStringAttribute->toFloat());
		CPPUNIT_ASSERT_EQUAL(static_cast<double>(0), genericStringAttribute->toDouble());
		CPPUNIT_ASSERT_EQUAL(value, genericStringAttribute->toString());
	}
}
