/*
	This file is part of the Util library.
	Copyright (C) 2012-2013 Benjamin Eikel <benjamin@eikel.org>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifndef FILEUTILSTEST_H_
#define FILEUTILSTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class FileUtilsTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(FileUtilsTest);
	CPPUNIT_TEST(testFS);
#ifdef UTIL_HAVE_LIB_ARCHIVE
	CPPUNIT_TEST(testARCHIVE);
#endif
#ifdef UTIL_HAVE_LIB_SQLITE
	CPPUNIT_TEST(testDBFS);
#endif
#ifdef UTIL_HAVE_LIB_ZIP
	CPPUNIT_TEST(testZIP);
#endif
	CPPUNIT_TEST_SUITE_END();

	public:
#ifdef UTIL_HAVE_LIB_ARCHIVE
		void testARCHIVE();
#endif
		void testFS();
#ifdef UTIL_HAVE_LIB_SQLITE
		void testDBFS();
#endif
#ifdef UTIL_HAVE_LIB_ZIP
		void testZIP();
#endif
};

#endif /* FILEUTILSTEST_H_ */
