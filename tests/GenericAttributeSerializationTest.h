/*
	This file is part of the Util library.
	Copyright (C) 2012-2015 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef GENERICATTRIBUTESERIALIZATIONTEST_H_
#define GENERICATTRIBUTESERIALIZATIONTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class GenericAttributeSerializationTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GenericAttributeSerializationTest);
	CPPUNIT_TEST(testStandardSerialization);
	CPPUNIT_TEST(testListSerialization);
	CPPUNIT_TEST(testMapSerialization);
	CPPUNIT_TEST(testNestedSerialization);
	CPPUNIT_TEST(testCustomSerialization);
	CPPUNIT_TEST(testCustomWithContextSerialization);
	CPPUNIT_TEST_SUITE_END();

	public:
		void testStandardSerialization();
		void testListSerialization();
		void testMapSerialization();
		void testNestedSerialization();
		void testCustomSerialization();
		void testCustomWithContextSerialization();
};

#endif /* GENERICATTRIBUTESERIALIZATIONTEST_H_ */
