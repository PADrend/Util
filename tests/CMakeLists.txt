#
# This file is part of the Util library.
# Copyright (C) 2011-2013 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
cmake_minimum_required(VERSION 2.8.11)

option(UTIL_BUILD_TESTS "Defines if CppUnit tests for the Util library are built.")
if(UTIL_BUILD_TESTS)
	add_executable(UtilTest 
		BidirectionalMapTest.cpp
		ConcurrencyTest.cpp
		EncodingTest.cpp
		FactoryTest.cpp
		FileUtilsTest.cpp
		GenericAttributeSerializationTest.cpp
		GenericAttributeTest.cpp
		GenericConversionTest.cpp
		GenericTest.cpp
		NetworkTest.cpp
		StringUtilsTest.cpp
		TimerTest.cpp
		TriStateTest.cpp
		UpdatableHeapTest.cpp
		WrapperFactoryTest.cpp
		ZIPTest.cpp
		UtilTestMain.cpp
	)

	target_link_libraries(UtilTest LINK_PRIVATE Util)

	# Dependency to CppUnit
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)
	find_package(CppUnit REQUIRED)
	target_include_directories(UtilTest PRIVATE ${CPPUNIT_INCLUDE_DIRS})
	target_link_libraries(UtilTest LINK_PRIVATE ${CPPUNIT_LIBRARIES})

	if(COMPILER_SUPPORTS_CXX11)
		set_property(TARGET UtilTest APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++11 ")
	elseif(COMPILER_SUPPORTS_CXX0X)
		set_property(TARGET UtilTest APPEND_STRING PROPERTY COMPILE_FLAGS "-std=c++0x ")
	endif()

	install(TARGETS UtilTest
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT tests
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
	)

	enable_testing()
	add_test(NAME UtilUnitTests COMMAND UtilTest)
endif()
