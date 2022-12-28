/*
	This file is part of the Util library.
	Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "StringUtils.h"
#include <catch2/catch.hpp>
#include <cstdint>
#include <string>

#ifdef _MSC_VER
#pragma warning(push)
#pragma warning(disable : 4146) // unary minus operator applied to unsigned type, result still unsigned
#endif

#define NUMBER_TEST(_TYPE, _VALUE) \
{\
	const _TYPE number = _VALUE;\
	REQUIRE(std::string(#_VALUE) == Util::StringUtils::toString<_TYPE>(number));\
}

TEST_CASE("StringUtilsTest", "[StringUtilsTest]") {
	NUMBER_TEST(uint64_t, 0)
	NUMBER_TEST(uint64_t, 1)
	NUMBER_TEST(uint64_t, 10)
	NUMBER_TEST(uint64_t, 100)
	NUMBER_TEST(uint64_t, 18446744073709551615)
	
	NUMBER_TEST(int64_t, -9223372036854775808)
	NUMBER_TEST(int64_t, -100)
	NUMBER_TEST(int64_t, -10)
	NUMBER_TEST(int64_t, -1)
	NUMBER_TEST(int64_t, 0)
	NUMBER_TEST(int64_t, 1)
	NUMBER_TEST(int64_t, 10)
	NUMBER_TEST(int64_t, 100)
	NUMBER_TEST(int64_t, 9223372036854775807)
	
	NUMBER_TEST(uint32_t, 0)
	NUMBER_TEST(uint32_t, 1)
	NUMBER_TEST(uint32_t, 10)
	NUMBER_TEST(uint32_t, 100)
	NUMBER_TEST(uint32_t, 4294967295)
	
	NUMBER_TEST(int32_t, -2147483648)
	NUMBER_TEST(int32_t, -100)
	NUMBER_TEST(int32_t, -10)
	NUMBER_TEST(int32_t, -1)
	NUMBER_TEST(int32_t, 0)
	NUMBER_TEST(int32_t, 1)
	NUMBER_TEST(int32_t, 10)
	NUMBER_TEST(int32_t, 100)
	NUMBER_TEST(int32_t, 2147483647)
	
	NUMBER_TEST(uint16_t, 0)
	NUMBER_TEST(uint16_t, 1)
	NUMBER_TEST(uint16_t, 10)
	NUMBER_TEST(uint16_t, 100)
	NUMBER_TEST(uint16_t, 65535)
	
	NUMBER_TEST(int16_t, -32768)
	NUMBER_TEST(int16_t, -100)
	NUMBER_TEST(int16_t, -10)
	NUMBER_TEST(int16_t, -1)
	NUMBER_TEST(int16_t, 0)
	NUMBER_TEST(int16_t, 1)
	NUMBER_TEST(int16_t, 10)
	NUMBER_TEST(int16_t, 100)
	NUMBER_TEST(int16_t, 32767)
	
	NUMBER_TEST(uint8_t, 0)
	NUMBER_TEST(uint8_t, 1)
	NUMBER_TEST(uint8_t, 10)
	NUMBER_TEST(uint8_t, 100)
	NUMBER_TEST(uint8_t, 255)
	
	NUMBER_TEST(int8_t, -128)
	NUMBER_TEST(int8_t, -100)
	NUMBER_TEST(int8_t, -10)
	NUMBER_TEST(int8_t, -1)
	NUMBER_TEST(int8_t, 0)
	NUMBER_TEST(int8_t, 1)
	NUMBER_TEST(int8_t, 10)
	NUMBER_TEST(int8_t, 100)
	NUMBER_TEST(int8_t, 127)
}

#ifdef _MSC_VER
#pragma warning(pop)
#endif