/*
	This file is part of the Util library.
	Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef NETWORKTEST_H_
#define NETWORKTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class NetworkTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(NetworkTest);
	CPPUNIT_TEST(testTCP);
	CPPUNIT_TEST(testUDP);
	CPPUNIT_TEST(testUDPgetPort);
	CPPUNIT_TEST(testDataConnection);
// 	CPPUNIT_TEST(testTCPClientExit);
	CPPUNIT_TEST_SUITE_END();

	public:
		void testTCP();
		void testUDP();
		void testUDPgetPort();
		void testDataConnection();
		void testTCPClientExit();
};

#endif /* NETWORKTEST_H_ */
