# Try to find SQLite3. Once done, this will define:
#
#   SQLITE3_FOUND - variable which returns the result of the search
#   SQLITE3_INCLUDE_DIRS - list of include directories
#   SQLITE3_LIBRARIES - options for the linker

#=============================================================================
# Copyright 2012 Benjamin Eikel
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
pkg_check_modules(PC_SQLITE3 QUIET sqlite3)

find_path(SQLITE3_INCLUDE_DIR
	sqlite3.h
	HINTS ${PC_SQLITE3_INCLUDEDIR} ${PC_SQLITE3_INCLUDE_DIRS}
)
find_library(SQLITE3_LIBRARY
	NAMES sqlite3 SQLite
	HINTS ${PC_SQLITE3_LIBDIR} ${PC_SQLITE3_LIBRARY_DIRS}
)

set(SQLITE3_INCLUDE_DIRS ${SQLITE3_INCLUDE_DIR})
set(SQLITE3_LIBRARIES ${SQLITE3_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(SQLite3 DEFAULT_MSG
	SQLITE3_INCLUDE_DIR
	SQLITE3_LIBRARY
)

mark_as_advanced(
	SQLITE3_INCLUDE_DIR
	SQLITE3_LIBRARY
)
