/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef GENERICTEST_H_
#define GENERICTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class GenericTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(GenericTest);
	CPPUNIT_TEST(testBasic);
	CPPUNIT_TEST(testArray);
	CPPUNIT_TEST(testMap);
	CPPUNIT_TEST_SUITE_END();

	public:
		void testBasic();
		void testArray();
		void testMap();
};

#endif /* GENERICTEST_H_ */
