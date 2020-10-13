/*
	This file is part of the Util library.
	Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
	
	This library is subject to the terms of the Mozilla Public License, v. 2.0.
	You should have received a copy of the MPL along with this library; see the 
	file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
*/
#ifdef UTIL_HAVE_LIB_VULKAN

#ifndef VULKANWINDOWTEST_H_
#define VULKANWINDOWTEST_H_

#include <cppunit/TestFixture.h>
#include <cppunit/extensions/HelperMacros.h>

class VulkanWindowTest : public CppUnit::TestFixture {
	CPPUNIT_TEST_SUITE(VulkanWindowTest);
	CPPUNIT_TEST(test);
	CPPUNIT_TEST_SUITE_END();

	public:
		void test();
};

#endif /* VULKANWINDOWTEST_H_ */
#endif /* UTIL_HAVE_LIB_VULKAN */