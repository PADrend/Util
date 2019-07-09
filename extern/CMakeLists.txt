#
# This file is part of the Util library.
# Copyright (C) 2018-2019 Sascha Brandt <sascha@brandt.graphics>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#
include(FetchContent)

# ------------------------------------------------------------------------------
# A modern, C++-native, header-only, test framework for unit-tests,
# TDD and BDD - using C++11, C++14, C++17 and later

if(UTIL_BUILD_TESTS)
  set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
  set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
  set(CATCH_INSTALL_HELPERS OFF CACHE BOOL "" FORCE)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.2.3
  )

  FetchContent_GetProperties(catch2)
  if(NOT catch2_POPULATED)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "Fetching Catch2...")
    FetchContent_Populate(catch2)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR} EXCLUDE_FROM_ALL)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
  endif()
  
  #set(catch2_SOURCE_DIR "${catch2_SOURCE_DIR}" PARENT_SCOPE)
endif(UTIL_BUILD_TESTS)
