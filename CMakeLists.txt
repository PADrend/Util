#
# This file is part of the Util library.
# Copyright (C) 2009-2013 Benjamin Eikel <benjamin@eikel.org>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
cmake_minimum_required(VERSION 3.1.0)
project(Util VERSION 0.2.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Set up install directories
include(GNUInstallDirs)
set(CMAKE_INSTALL_CMAKECONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/Util)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

include(SubdirectorySources)
add_sources(Util
	Encoding.cpp
	GenericAttribute.cpp
	GenericAttributeSerialization.cpp
	GenericConversion.cpp
	JSON_Parser.cpp
	LibRegistry.cpp
	Macros.cpp
	MicroXML.cpp
	ProgressIndicator.cpp
	StringIdentifier.cpp
	StringUtils.cpp
	Timer.cpp
	TypeConstant.cpp
	Util.cpp
	Utils.cpp
)
add_subdirectory(Graphics)
add_subdirectory(IO)
add_subdirectory(Network)
add_subdirectory(Serialization)
add_subdirectory(UI)
add_subdirectory(examples)

get_property(Util_SOURCES GLOBAL PROPERTY Util_SRCS)

add_library(Util ${Util_SOURCES})

if(WIN32)
	target_link_libraries(Util PRIVATE "-lpsapi -gdi32")
endif()
if(UNIX AND NOT APPLE)
	target_link_libraries(Util PRIVATE rt)
endif()

# Dependency to SQLite3
find_package(SQLite3)
if(SQLITE3_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_SQLITE)
	target_include_directories(Util PRIVATE ${SQLITE3_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${SQLITE3_LIBRARIES})
endif()

# Dependency to libpng
find_package(PNG)
if(PNG_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_PNG)
	target_compile_definitions(Util PRIVATE "${PNG_DEFINITIONS}")
	target_include_directories(Util PRIVATE ${PNG_INCLUDE_DIR})
	target_link_libraries(Util PRIVATE ${PNG_LIBRARIES})
endif()

# Dependency to libarchive
find_package(LibArchive)
if(LibArchive_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_ARCHIVE)
	target_include_directories(Util PRIVATE ${LibArchive_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${LibArchive_LIBRARIES})
endif()

# Dependency to libzip
find_package(libzip)
if(LIBZIP_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_ZIP)
	target_include_directories(Util PRIVATE ${LIBZIP_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${LIBZIP_LIBRARIES})
endif()

# Dependency to libxml2
find_package(LibXml2)
if(LIBXML2_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_XML2)
	target_compile_definitions(Util PRIVATE "${LIBXML2_DEFINITIONS}")
	target_include_directories(Util PRIVATE ${LIBXML2_INCLUDE_DIR})
	target_link_libraries(Util PRIVATE ${LIBXML2_LIBRARIES})
endif()

# Dependency to libcurl
find_package(CURL)
if(CURL_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_CURL)
	target_include_directories(Util PRIVATE ${CURL_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${CURL_LIBRARIES})
endif()

# Dependency to FreeType
find_package(Freetype)
if(FREETYPE_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_FREETYPE)
	target_include_directories(Util PRIVATE ${FREETYPE_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${FREETYPE_LIBRARIES})
	
	# Dependency to ZLIB
	find_package(ZLIB QUIET)
	if(ZLIB_FOUND)
		target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_ZLIB)
		target_include_directories(Util PRIVATE ${ZLIB_INCLUDE_DIRS})
		target_link_libraries(Util PRIVATE ${ZLIB_LIBRARIES})
	endif()
endif()

# Dependency to SDL2
set(SDL2_BUILDING_LIBRARY ON)
find_package(SDL2 2.0.0)
if(SDL2_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_SDL2)
	target_include_directories(Util PRIVATE ${SDL2_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${SDL2_LIBRARIES})
endif()

# Dependency to SDL2_image
find_package(SDL2_image 2.0.0)
if(SDL2_IMAGE_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_SDL2_IMAGE)
	target_include_directories(Util PRIVATE ${SDL2_IMAGE_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${SDL2_IMAGE_LIBRARIES})
endif()

# Dependency to SDL2_net
find_package(SDL2_net 2.0.0)
if(SDL2_NET_FOUND)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_SDL2_NET)
	target_include_directories(Util PRIVATE ${SDL2_NET_INCLUDE_DIRS})
	target_link_libraries(Util PRIVATE ${SDL2_NET_LIBRARIES})
endif()

# Dependency to pthread
set(CMAKE_THREAD_PREFER_PTHREAD ON)
find_package(Threads)
if(CMAKE_USE_PTHREADS_INIT AND (NOT DEFINED XCODE_VERSION OR XCODE_VERSION LESS 5.0))
	target_link_libraries(Util PRIVATE ${CMAKE_THREAD_LIBS_INIT})
endif()

# Only use X11 on Linux. There were problems with X11 on Mac OS X
if(CMAKE_SYSTEM_NAME STREQUAL Linux)
	# Dependency to X11
	find_package(X11)
	if(X11_FOUND)
		target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_X11)
		target_include_directories(Util PRIVATE ${X11_INCLUDE_DIR})
		target_link_libraries(Util PRIVATE ${X11_LIBRARIES})
		
		option(UTIL_X11_JOYSTICK_SUPPORT "Defines if joystick support for X11 should be included.")
		if(UTIL_X11_JOYSTICK_SUPPORT)
			target_compile_definitions(Util PRIVATE UTIL_X11_JOYSTICK_SUPPORT)
		endif()
			
	endif()
	
	
	# Dependency to EGL
	find_package(EGL)
	if(EGL_FOUND)
		target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_EGL)
		target_include_directories(Util PRIVATE ${EGL_INCLUDE_DIRS})
		target_link_libraries(Util PRIVATE ${EGL_LIBRARIES})
	endif()

	# Dependency to GLX
	find_package(GLX)
	if(GLX_FOUND)
		target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_GLX)
		target_include_directories(Util PRIVATE ${GLX_INCLUDE_DIRS})
		target_link_libraries(Util PRIVATE ${GLX_LIBRARIES})

		include(CheckCXXSymbolExists)
		set(CMAKE_REQUIRED_INCLUDES ${GLX_INCLUDE_DIRS})
		set(CMAKE_REQUIRED_LIBRARIES ${GLX_LIBRARIES})
		check_cxx_symbol_exists(glXGetProcAddressARB "GL/glx.h" UTIL_HAVE_GLX_GETPROCADDRESSARB)
		if(UTIL_HAVE_GLX_GETPROCADDRESSARB)
			target_compile_definitions(Util PRIVATE UTIL_HAVE_GLX_GETPROCADDRESSARB)
		endif()
	endif()
endif()

include(CheckIncludeFileCXX)
include(CheckCXXSymbolExists)

# Test if the header execinfo.h exists
check_include_file_cxx(execinfo.h UTIL_HAVE_EXECINFO_H)
if(UTIL_HAVE_EXECINFO_H)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_EXECINFO_H)
endif()

# Test if the header malloc.h exists
check_include_file_cxx(malloc.h UTIL_HAVE_MALLOC_H)
if(UTIL_HAVE_MALLOC_H)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_MALLOC_H)
endif()
# Test if mallinfo exists
check_cxx_symbol_exists(mallinfo malloc.h UTIL_HAVE_MALLINFO)
if(UTIL_HAVE_MALLINFO)
	target_compile_definitions(Util PRIVATE UTIL_HAVE_MALLINFO)
endif()
# Test if malloc_info exists
check_cxx_symbol_exists(malloc_info malloc.h UTIL_HAVE_MALLOC_INFO)
if(UTIL_HAVE_MALLOC_INFO)
	set_property(TARGET Util APPEND PROPERTY COMPILE_DEFINITIONS UTIL_HAVE_MALLOC_INFO)
endif()

# Test if open_memstream exists
check_cxx_symbol_exists(open_memstream stdio.h UTIL_HAVE_OPEN_MEMSTREAM)
if(UTIL_HAVE_OPEN_MEMSTREAM)
	set_property(TARGET Util APPEND PROPERTY COMPILE_DEFINITIONS UTIL_HAVE_OPEN_MEMSTREAM)
endif()

# Set version of library
set_target_properties(Util PROPERTIES VERSION ${Util_VERSION}
                                      SOVERSION ${Util_VERSION_MAJOR})

include(FeatureSummary)
set_package_properties(CURL PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Support for using the http:// and ftp:// virtual file system."
)
set_package_properties(EGL PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Creation of a window and a rendering context for platforms supporting EGL."
)
set_package_properties(Freetype PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Creation of a bitmap containing a font's glyphs."
)
set_package_properties(GLX PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Creation of a rendering context for platforms supporting GLX. Has to be used together with X11."
)
set_package_properties(LibArchive PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Support for using the virtual file systems for archives (e.g. zip, tar)."
)
set_package_properties(libzip PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Support for using the zip:// virtual file system."
)
set_package_properties(LibXml2 PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Replace the provided XML parser by libxml2's parser."
)
set_package_properties(PNG PROPERTIES
	TYPE RECOMMENDED
	PURPOSE "Read/write support for PNG (Portable Network Graphics) raster image files."
)
set_package_properties(SDL2 PROPERTIES
	TYPE RECOMMENDED
	PURPOSE "Multi-platform creation of a window and a rendering context. Multi-platform concurrency support."
)
set_package_properties(SDL2_image PROPERTIES
	TYPE RECOMMENDED
	PURPOSE "Read/write support for additional raster image file formats (e.g. JPEG, TIFF)."
)
set_package_properties(SDL2_net PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Multi-platform support for network communication."
)
set_package_properties(SQLite3 PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Support for using the dbfs:// virtual file system."
)
set_package_properties(X11 PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Creation of a window for platforms using X11."
)
set_package_properties(ZLIB PROPERTIES
	TYPE OPTIONAL
	PURPOSE "Helper library required by other external libraries (e.g. libpng, libzip)."
)
feature_summary(DESCRIPTION "The following packages have been found:" WHAT PACKAGES_FOUND)
feature_summary(DESCRIPTION "The following packages have *not* been found:" WHAT PACKAGES_NOT_FOUND)

add_subdirectory(tests)

# Install the header files
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Concurrency/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Concurrency COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Concurrency/DataStructures/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Concurrency/DataStructures COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Factory/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Factory COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Graphics/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Graphics COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/IO/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/IO COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Network/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Network COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/Serialization/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/Serialization COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/UI/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util/UI COMPONENT headers)
file(GLOB UTIL_HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/*.h")
install(FILES ${UTIL_HEADERS} DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/Util COMPONENT headers)

# Install the library
install(TARGETS Util EXPORT LibraryExport
	RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} COMPONENT runtimelibraries
	LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT runtimelibraries
	ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} COMPONENT developmentlibraries
)

# Set include directories for users of this library
target_include_directories(Util
                           INTERFACE
                           "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/..>"
                           "$<INSTALL_INTERFACE:$<INSTALL_PREFIX>/${CMAKE_INSTALL_INCLUDEDIR}>")

# Library export
install(EXPORT LibraryExport DESTINATION ${CMAKE_INSTALL_CMAKECONFIGDIR} FILE UtilTargets.cmake COMPONENT developmentlibraries)

# Create the configuration files
include(CMakePackageConfigHelpers)
configure_package_config_file(UtilConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/UtilConfig.cmake
                              INSTALL_DESTINATION ${CMAKE_INSTALL_CMAKECONFIGDIR}
							  PATH_VARS CMAKE_INSTALL_CMAKECONFIGDIR
							  NO_SET_AND_CHECK_MACRO
							  NO_CHECK_REQUIRED_COMPONENTS_MACRO)
write_basic_package_version_file(${CMAKE_CURRENT_BINARY_DIR}/UtilConfigVersion.cmake
                                 VERSION ${UTIL_VERSION_STRING}
                                 COMPATIBILITY SameMajorVersion)

# Install the configuration files
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/UtilConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/UtilConfigVersion.cmake
        DESTINATION ${CMAKE_INSTALL_CMAKECONFIGDIR} COMPONENT developmentlibraries)

# Packaging
include(InstallRequiredSystemLibraries)

set(CPACK_PACKAGE_NAME "libutil${Util_VERSION_MAJOR}")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Util library")
set(CPACK_PACKAGE_VENDOR "Benjamin Eikel; Claudius Jaehn; Ralf Petring")
set(CPACK_PACKAGE_CONTACT "Benjamin Eikel <benjamin@eikel.org>")
set(CPACK_PACKAGE_VERSION_MAJOR ${Util_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${Util_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${Util_VERSION_PATCH})
set(CPACK_PACKAGE_FILE_NAME "libUtil")
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/LICENSE")
set(CPACK_STRIP_FILES ON)

include(CPack)
