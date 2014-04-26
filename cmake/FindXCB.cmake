# Try to find xcb. Once done, this will define:
#
#   XCB_FOUND - variable which returns the result of the search
#   XCB_INCLUDE_DIRS - list of include directories
#   XCB_LIBRARIES - options for the linker

#=============================================================================
# Copyright 2014 Benjamin Eikel
#
# Distributed under the OSI-approved BSD License (the "License");
# see accompanying file Copyright.txt for details.
#
# This software is distributed WITHOUT ANY WARRANTY; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
# See the License for more information.
#=============================================================================
# (To distribute this file outside of CMake, substitute the full
#  License text for the above reference.)

find_package(PkgConfig)
pkg_check_modules(PC_XCB QUIET xcb)

find_path(XCB_INCLUDE_DIR
	xcb/xcb.h
	HINTS ${PC_XCB_INCLUDEDIR} ${PC_XCB_INCLUDE_DIRS}
)
find_library(XCB_LIBRARY
	xcb
	HINTS ${PC_XCB_LIBDIR} ${PC_XCB_LIBRARY_DIRS}
)

set(XCB_INCLUDE_DIRS ${XCB_INCLUDE_DIR})
set(XCB_LIBRARIES ${XCB_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XCB DEFAULT_MSG
	XCB_INCLUDE_DIR
	XCB_LIBRARY
)

mark_as_advanced(
	XCB_INCLUDE_DIR
	XCB_LIBRARY
)
