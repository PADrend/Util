/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef REGISTRYTEST_H_
#define REGISTRYTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class RegistryTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(RegistryTest);
	CPPUNIT_TEST(testList);
	CPPUNIT_TEST(testMultiset);
	CPPUNIT_TEST(testFunctions);
	CPPUNIT_TEST_SUITE_END();

	public:
		void testList();
		void testMultiset();
		void testFunctions();
};

#endif /* REGISTRYTEST_H_ */
