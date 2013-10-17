/*
	This file is part of the Util library.
	Copyright (C) 2013 Benjamin Eikel <benjamin@eikel.org>
	Copyright (C) 2013 Ralf Petring <ralf@petring.net>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#include "TriStateTest.h"

#include <Util/TriState.h>

CPPUNIT_TEST_SUITE_REGISTRATION(TriStateTest);


void TriStateTest::test() {
	
	using Util::TriState;

	CPPUNIT_ASSERT(sizeof(TriState)==1);

	TriState x;
	CPPUNIT_ASSERT(x==TriState());
	CPPUNIT_ASSERT(x!=true);
	CPPUNIT_ASSERT(x!=false);
	CPPUNIT_ASSERT(!x.isTrue());
	CPPUNIT_ASSERT(!x.isFalse());
	CPPUNIT_ASSERT(x.isUndefined());
	CPPUNIT_ASSERT(!x.isDefined());

	TriState y(true);
	CPPUNIT_ASSERT(y!=TriState());
	CPPUNIT_ASSERT(y!=x);
	CPPUNIT_ASSERT(y==true);
	CPPUNIT_ASSERT(y.isTrue());
	CPPUNIT_ASSERT(y!=false);
	CPPUNIT_ASSERT(!y.isUndefined());
	CPPUNIT_ASSERT(y.isDefined());

	CPPUNIT_ASSERT(TriState(y) == y);
	CPPUNIT_ASSERT(TriState(x) != y);

	y.undefine();
	CPPUNIT_ASSERT(y==x);

	y = true;
	CPPUNIT_ASSERT(x!=y);
	x = false;
	CPPUNIT_ASSERT(x.isFalse());
	CPPUNIT_ASSERT(x!=y);
	y = false;
	CPPUNIT_ASSERT(x==y);
}
