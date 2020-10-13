#
# This file is part of the Util library.
# Copyright (C) 2011-2015 Benjamin Eikel <benjamin@eikel.org>
# Copyright (C) 2019 Sascha Brandt <sascha@brandt.graphics>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
cmake_minimum_required(VERSION 3.1.0)
project(UtilTest)

if(UTIL_BUILD_TESTS)
	add_executable(UtilTest 
		BidirectionalMapTest.cpp
		EncodingTest.cpp
		FactoryTest.cpp
		FileUtilsTest.cpp
		GenericAttributeSerializationTest.cpp
		GenericAttributeTest.cpp
		GenericConversionTest.cpp
		GenericTest.cpp
		NetProviderTest.cpp
		NetworkTest.cpp
		RegistryTest.cpp
		StringUtilsTest.cpp
		TimerTest.cpp
		TriStateTest.cpp
		UpdatableHeapTest.cpp
		VulkanWindowTest.cpp
		WrapperFactoryTest.cpp
		ZIPTest.cpp
		UtilTestMain.cpp
	)
		
	target_link_libraries(UtilTest PRIVATE Util)
	set_target_properties(UtilTest PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}")

	target_link_libraries(UtilTest PRIVATE UtilExtern)
	
	set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_LIST_DIR}/cmake)
		
	# Dependency to pthread
	set(CMAKE_THREAD_PREFER_PTHREAD ON)
	find_package(Threads)
	if(CMAKE_USE_PTHREADS_INIT AND (NOT DEFINED XCODE_VERSION OR XCODE_VERSION LESS 5.0))
		target_link_libraries(UtilTest PRIVATE ${CMAKE_THREAD_LIBS_INIT})
	endif()
	

	# ------------------------------------------------------------------------------
	# Catch2
	if(${CMAKE_VERSION} VERSION_LESS "3.11.0") 
		include(FetchContentFallback)
	else()
		include(FetchContent)
	endif()
	
	FetchContent_Declare(
		catch2
		GIT_REPOSITORY https://github.com/catchorg/Catch2.git
		GIT_TAG        v2.9.1
	)

	FetchContent_GetProperties(catch2)
	if(NOT catch2_POPULATED)
		message(STATUS "Fetching Catch2...")
		FetchContent_Populate(catch2)
		# hide options in cmake-gui
		set(CATCH_USE_VALGRIND OFF CACHE INTERNAL "" FORCE)
		set(CATCH_BUILD_TESTING OFF CACHE INTERNAL "" FORCE)
		set(CATCH_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
		set(CATCH_BUILD_EXTRA_TESTS OFF CACHE INTERNAL "" FORCE)
		set(CATCH_ENABLE_COVERAGE OFF CACHE INTERNAL "" FORCE)
		set(CATCH_ENABLE_WERROR ON CACHE INTERNAL "" FORCE)
		set(CATCH_INSTALL_DOCS OFF CACHE INTERNAL "" FORCE)
		set(CATCH_INSTALL_HELPERS OFF CACHE INTERNAL "" FORCE)
		add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR} EXCLUDE_FROM_ALL)
	endif()
	target_link_libraries(UtilTest PRIVATE Catch2::Catch2)
			
	# Dependency to Vulkan
	find_package(Vulkan)
	if(Vulkan_FOUND)
		target_compile_definitions(UtilTest PRIVATE UTIL_HAVE_LIB_VULKAN)
		target_include_directories(UtilTest PRIVATE ${Vulkan_INCLUDE_DIRS})
		target_link_libraries(UtilTest LINK_PRIVATE ${Vulkan_LIBRARIES})

		# ------------------------------------------------------------------------------
		# shaderc (glsl compiler required by vulkan)
		find_package(shaderc)
		target_include_directories(UtilTest PRIVATE ${SHADERC_INCLUDE_DIRS})
		target_link_libraries(UtilTest LINK_PRIVATE ${SHADERC_LIBRARIES})
		
		#FetchContent_Declare(
			#	shaderc
			#	GIT_REPOSITORY https://github.com/google/shaderc.git
			#	GIT_TAG        v2019.0
		#)

		#FetchContent_GetProperties(shaderc)
		#if(NOT shaderc_POPULATED)
		#	message(STATUS "Fetching shaderc...")
		#	FetchContent_Populate(shaderc)
		#	set(SHADERC_SKIP_INSTALL ON CACHE INTERNAL "" FORCE)
		#	set(SHADERC_SKIP_TESTS ON CACHE INTERNAL "" FORCE)
		#	add_subdirectory(${shaderc_SOURCE_DIR} ${shaderc_BINARY_DIR} EXCLUDE_FROM_ALL)
	#endif()
		#target_link_libraries(UtilTest INTERFACE shaderc)
		#set_target_properties(shaderc PROPERTIES POSITION_INDEPENDENT_CODE ON)
	endif()

	install(TARGETS UtilTest
		RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT tests
		LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
		ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT tests
	)
	
	configure_file(${CMAKE_CURRENT_LIST_DIR}/CTestCustom.cmake ${CMAKE_BINARY_DIR})
	
	enable_testing()
	add_test(NAME BidirectionalMapTest COMMAND UtilTest [BidirectionalMapTest])
	add_test(NAME EncodingTest COMMAND UtilTest [EncodingTest])
	add_test(NAME FactoryTest COMMAND UtilTest [FactoryTest])
	add_test(NAME FileUtilsTest COMMAND UtilTest [FileUtilsTest])
	add_test(NAME GenericAttributeSerializationTest COMMAND UtilTest [GenericAttributeSerializationTest])
	add_test(NAME GenericAttributeTest COMMAND UtilTest [GenericAttributeTest])
	add_test(NAME GenericConversionTest COMMAND UtilTest [GenericConversionTest])
	add_test(NAME GenericTest COMMAND UtilTest [GenericTest])
	add_test(NAME HttpTest COMMAND UtilTest [HttpTest])
	add_test(NAME NetworkTest COMMAND UtilTest [NetworkTest])
	add_test(NAME RegistryTest COMMAND UtilTest [RegistryTest])
	add_test(NAME StringUtilsTest COMMAND UtilTest [StringUtilsTest])
	#add_test(NAME TimerTest COMMAND UtilTest [TimerTest])
	add_test(NAME TriStateTest COMMAND UtilTest [TriStateTest])
	add_test(NAME UpdatableHeapTest COMMAND UtilTest [UpdatableHeapTest])
	add_test(NAME WrapperFactoryTest COMMAND UtilTest [WrapperFactoryTest])
	add_test(NAME ZIPTest COMMAND UtilTest [ZIPTest])
endif()
