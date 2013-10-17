/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "GenericConversionTest.h"
#include <cppunit/TestAssert.h>
#include <Util/Generic.h>
#include <Util/GenericConversion.h>
#include <sstream>
#include <string>
CPPUNIT_TEST_SUITE_REGISTRATION(GenericConversionTest);

static const std::string testJSONData =
R"({
	"1"		:	2,
	"foo"	:	"bar",
	"true"	:	false,
	"bar"	:	true,
	"t\\\""	:	"test escape",
	"a"		:	[1, 02e10, 3.0, -2.5, "27", "a\\\"a"]
})";

void GenericConversionTest::testJSON() {
	std::istringstream inputStream(testJSONData);
	auto genericFirst = Util::GenericConversion::fromJSON(inputStream);
	
	std::stringstream tempStream;
	Util::GenericConversion::toJSON(genericFirst, tempStream);

	const std::string firstOut = tempStream.str();

	auto genericSecond = Util::GenericConversion::fromJSON(tempStream);

	std::ostringstream outputStream;
	Util::GenericConversion::toJSON(genericSecond, outputStream);

	const std::string secondOut = outputStream.str();
	CPPUNIT_ASSERT_EQUAL(firstOut, secondOut);
}
