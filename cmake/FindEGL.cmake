# Try to find EGL. Once done, this will define:
#
#   EGL_FOUND - variable which returns the result of the search
#   EGL_INCLUDE_DIRS - list of include directories
#   EGL_LIBRARIES - options for the linker

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
pkg_check_modules(PC_EGL QUIET egl)

find_path(EGL_INCLUDE_DIR
	EGL/egl.h
	HINTS ${PC_EGL_INCLUDEDIR} ${PC_EGL_INCLUDE_DIRS}
)
find_library(EGL_LIBRARY
	EGL
	HINTS ${PC_EGL_LIBDIR} ${PC_EGL_LIBRARY_DIRS}
)

set(EGL_INCLUDE_DIRS ${EGL_INCLUDE_DIR})
set(EGL_LIBRARIES ${EGL_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(EGL DEFAULT_MSG
	EGL_INCLUDE_DIR
	EGL_LIBRARY
)

mark_as_advanced(
	EGL_INCLUDE_DIR
	EGL_LIBRARY
)
