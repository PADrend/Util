# Try to find XCB_KEYSYMS. Once done, this will define:
#
#   XCB_KEYSYMS_FOUND - variable which returns the result of the search
#   XCB_KEYSYMS_INCLUDE_DIRS - list of include directories
#   XCB_KEYSYMS_LIBRARIES - options for the linker

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
pkg_check_modules(PC_XCB_KEYSYMS QUIET xcb-keysyms)

find_path(XCB_KEYSYMS_INCLUDE_DIR
	xcb/xcb_keysyms.h
	HINTS ${PC_XCB_KEYSYMS_INCLUDEDIR} ${PC_XCB_KEYSYMS_INCLUDE_DIRS}
)
find_library(XCB_KEYSYMS_LIBRARY
	xcb-keysyms
	HINTS ${PC_XCB_KEYSYMS_LIBDIR} ${PC_XCB_KEYSYMS_LIBRARY_DIRS}
)

set(XCB_KEYSYMS_INCLUDE_DIRS ${XCB_KEYSYMS_INCLUDE_DIR})
set(XCB_KEYSYMS_LIBRARIES ${XCB_KEYSYMS_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(XCB_KEYSYMS DEFAULT_MSG
	XCB_KEYSYMS_INCLUDE_DIR
	XCB_KEYSYMS_LIBRARY
)

mark_as_advanced(
	XCB_KEYSYMS_INCLUDE_DIR
	XCB_KEYSYMS_LIBRARY
)
