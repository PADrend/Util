/*
	This file is part of the Util library.
	Copyright (C) 2012 Claudius Jähn <claudius@uni-paderborn.de>
	Copyright (C) 2012 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef BIDIRECTIONALUNORDEREDMAPTEST_H_
#define BIDIRECTIONALUNORDEREDMAPTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class BidirectionalUnorderedMapTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(BidirectionalUnorderedMapTest);
	CPPUNIT_TEST(test);
	CPPUNIT_TEST_SUITE_END();

	public:
		void test();
};

#endif /* ENCODINGTEST_H_ */
