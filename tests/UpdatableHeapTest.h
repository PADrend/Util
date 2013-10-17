/*
	This file is part of the Util library.
	Copyright (C) 2011-2012 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef UPDATABLETEST_H_
#define UPDATABLETEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class UpdatableHeapTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(UpdatableHeapTest);
	CPPUNIT_TEST(testSequence);
	CPPUNIT_TEST(testUpdate);
	CPPUNIT_TEST(testDelete);
	CPPUNIT_TEST(testDeleteUpdate);
	CPPUNIT_TEST_SUITE_END();

	public:
		void testSequence();
		void testUpdate();
		void testDelete();
		void testDeleteUpdate();
};

#endif /* UPDATABLETEST_H_ */
