/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include <catch2/catch.hpp>
#include "GenericAttribute.h"
#include <cstdint>
#include <memory>
#include <string>

static void testBoolGetters(const Util::BoolAttribute & attribute, bool expectedValue, const std::string & expectedString) {
	REQUIRE(expectedValue == attribute.get());
	REQUIRE(expectedValue == attribute.toBool());
	REQUIRE(static_cast<int>(expectedValue) == attribute.toInt());
	REQUIRE(static_cast<unsigned int>(expectedValue) == attribute.toUnsignedInt());
	REQUIRE(static_cast<float>(expectedValue) == attribute.toFloat());
	REQUIRE(static_cast<double>(expectedValue) == attribute.toDouble());
	REQUIRE(expectedString == attribute.toString());
}

TEST_CASE("GenericAttributeTest_testBool", "[GenericAttributeTest]") {
	const bool value = true;
	SECTION("Direct construction") { // Direct construction
		Util::BoolAttribute boolAttribute(value);
		testBoolGetters(boolAttribute, value, "true");

		const bool secondValue = false;
		boolAttribute.set(secondValue);
		testBoolGetters(boolAttribute, secondValue, "false");

		const bool thirdValue = true;
		boolAttribute = Util::BoolAttribute(thirdValue);
		testBoolGetters(boolAttribute, thirdValue, "true");
	}
	SECTION("Specific factory") { // Specific factory
		std::unique_ptr<Util::BoolAttribute> boolAttribute(Util::GenericAttribute::createBool(value));
		testBoolGetters(*boolAttribute, value, "true");

		const bool valueXor = value ^ value;
		boolAttribute->set(valueXor);
		testBoolGetters(*boolAttribute, valueXor, "false");

		std::unique_ptr<Util::BoolAttribute> clonedBoolAttribute(boolAttribute->clone());
		REQUIRE(*boolAttribute == *clonedBoolAttribute);
		boolAttribute.reset();

		testBoolGetters(*clonedBoolAttribute, valueXor, "false");
	}
	SECTION("Generic factory") { // Generic factory
		std::unique_ptr<Util::GenericAttribute> genericBoolAttribute(Util::GenericAttribute::create(value));
		REQUIRE(genericBoolAttribute->isA<Util::BoolAttribute>());
		REQUIRE(value == genericBoolAttribute->toType<Util::BoolAttribute>()->get());
		REQUIRE(true == genericBoolAttribute->toBool());
		REQUIRE(static_cast<int>(value) == genericBoolAttribute->toInt());
		REQUIRE(static_cast<unsigned int>(value) == genericBoolAttribute->toUnsignedInt());
		REQUIRE(static_cast<float>(value) == genericBoolAttribute->toFloat());
		REQUIRE(static_cast<double>(value) == genericBoolAttribute->toDouble());
		REQUIRE(std::string("true") == genericBoolAttribute->toString());
	}
}

template<typename attribute_t, typename val_t>
static void testNumberGetters(const attribute_t & attribute, val_t expectedValue, const std::string & expectedString) {
	REQUIRE(expectedValue == attribute.get());
	REQUIRE(true == attribute.toBool());
	REQUIRE(static_cast<int>(expectedValue) == attribute.toInt());
	REQUIRE(static_cast<unsigned int>(expectedValue) == attribute.toUnsignedInt());
	REQUIRE(static_cast<float>(expectedValue) == attribute.toFloat());
	REQUIRE(static_cast<double>(expectedValue) == attribute.toDouble());
	REQUIRE(expectedString == attribute.toString());
}

template<typename val_t>
static void typedNumberTest() {
	const val_t value = 5;
	SECTION("Direct construction") { // Direct construction
		Util::_NumberAttribute<val_t> numberAttribute(value);
		testNumberGetters(numberAttribute, value, "5");

		const val_t secondValue = 7;
		numberAttribute.set(secondValue);
		testNumberGetters(numberAttribute, secondValue, "7");

		const val_t thirdValue = 42;
		numberAttribute = Util::_NumberAttribute<val_t>(thirdValue);
		testNumberGetters(numberAttribute, thirdValue, "42");
	}
	SECTION("Specific factory") { // Specific factory
		std::unique_ptr<Util::_NumberAttribute<val_t>> numberAttribute(Util::GenericAttribute::createNumber<val_t>(value));
		testNumberGetters(*numberAttribute, value, "5");

		const val_t valueSquared = value * value;
		numberAttribute->set(valueSquared);
		testNumberGetters(*numberAttribute, valueSquared, "25");

		std::unique_ptr<Util::_NumberAttribute<val_t>> clonedNumberAttribute(numberAttribute->clone());
		REQUIRE(*numberAttribute == *clonedNumberAttribute);
		numberAttribute.reset();

		testNumberGetters(*clonedNumberAttribute, valueSquared, "25");
	}
	SECTION("Generic factory") { // Generic factory
		std::unique_ptr<Util::GenericAttribute> genericNumberAttribute(Util::GenericAttribute::create(value));
		REQUIRE(genericNumberAttribute->isA<Util::_NumberAttribute<val_t>>());
		REQUIRE(value == genericNumberAttribute->toType<Util::_NumberAttribute<val_t>>()->get());
		REQUIRE(true == genericNumberAttribute->toBool());
		REQUIRE(static_cast<int>(value) == genericNumberAttribute->toInt());
		REQUIRE(static_cast<unsigned int>(value) == genericNumberAttribute->toUnsignedInt());
		REQUIRE(static_cast<float>(value) == genericNumberAttribute->toFloat());
		REQUIRE(std::string("5") == genericNumberAttribute->toString());
	}
}

#define NUMBER_TEST(type) TEST_CASE("GenericAttributeTest_testNumber_" #type, "[GenericAttributeTest]") { typedNumberTest<type>(); }

NUMBER_TEST(char)
NUMBER_TEST(int8_t)
NUMBER_TEST(uint8_t)
NUMBER_TEST(int16_t)
NUMBER_TEST(uint16_t)
NUMBER_TEST(int32_t)
NUMBER_TEST(uint32_t)
NUMBER_TEST(int64_t)
NUMBER_TEST(uint64_t)
NUMBER_TEST(float)
NUMBER_TEST(double)

static void testStringGetters(const Util::StringAttribute & attribute, const std::string & expectedString, bool expectedBool, int expectedNumber) {
	REQUIRE(expectedString == attribute.get());
	REQUIRE(expectedBool == attribute.toBool());
	REQUIRE(static_cast<int>(expectedNumber) == attribute.toInt());
	REQUIRE(static_cast<unsigned int>(expectedNumber) == attribute.toUnsignedInt());
	REQUIRE(static_cast<float>(expectedNumber) == attribute.toFloat());
	REQUIRE(static_cast<double>(expectedNumber) == attribute.toDouble());
	REQUIRE(expectedString == attribute.toString());
}

TEST_CASE("GenericAttributeTest_testString", "[GenericAttributeTest]") {
	const std::string value = "abc";
	SECTION("Direct construction") { // Direct construction
		Util::StringAttribute stringAttribute(value);
		testStringGetters(stringAttribute, value, false, 0);

		const std::string secondValue = "28";
		stringAttribute.set(secondValue);
		testStringGetters(stringAttribute, secondValue, false, 28);

		const std::string thirdValue = "true";
		stringAttribute = Util::StringAttribute(thirdValue);
		testStringGetters(stringAttribute, thirdValue, true, 0);
	}
	SECTION("Specific factory") { // Specific factory
		std::unique_ptr<Util::StringAttribute> stringAttribute(Util::GenericAttribute::createString(value));
		testStringGetters(*stringAttribute, value, false, 0);

		const std::string valueDouble = value + value;
		stringAttribute->set(valueDouble);
		testStringGetters(*stringAttribute, valueDouble, false, 0);

		std::unique_ptr<Util::StringAttribute> clonedStringAttribute(stringAttribute->clone());
		REQUIRE(*stringAttribute == *clonedStringAttribute);
		stringAttribute.reset();

		testStringGetters(*clonedStringAttribute, valueDouble, false, 0);
	}
	SECTION("Generic factory") { // Generic factory
		std::unique_ptr<Util::GenericAttribute> genericStringAttribute(Util::GenericAttribute::create(value));
		REQUIRE(genericStringAttribute->isA<Util::StringAttribute>());
		REQUIRE(value == genericStringAttribute->toType<Util::StringAttribute>()->get());
		REQUIRE(false == genericStringAttribute->toBool());
		REQUIRE(static_cast<int>(0) == genericStringAttribute->toInt());
		REQUIRE(static_cast<unsigned int>(0) == genericStringAttribute->toUnsignedInt());
		REQUIRE(static_cast<float>(0) == genericStringAttribute->toFloat());
		REQUIRE(static_cast<double>(0) == genericStringAttribute->toDouble());
		REQUIRE(value == genericStringAttribute->toString());
	}
}
