#
# This file is part of the Util library.
# Copyright (C) 2018-2019 Sascha Brandt <sascha@brandt.graphics>
#
# This library is subject to the terms of the Mozilla Public License, v. 2.0.
# You should have received a copy of the MPL along with this library; see the 
# file LICENSE. If not, you can obtain one at http://mozilla.org/MPL/2.0/.
#

add_library(UtilExtern INTERFACE)

if(${CMAKE_VERSION} VERSION_LESS "3.11.0") 
  message("Please consider to switch to CMake 3.11.0")
  include(FetchContentFallback)
else()
  include(FetchContent)
endif()

# ------------------------------------------------------------------------------
# Catch2

if(UTIL_BUILD_TESTS)
  FetchContent_Declare(
    catch2
    GIT_REPOSITORY https://github.com/catchorg/Catch2.git
    GIT_TAG        v2.9.1
  )

  FetchContent_GetProperties(catch2)
  if(NOT catch2_POPULATED)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "Fetching Catch2...")
    FetchContent_Populate(catch2)
    set(CATCH_BUILD_TESTING OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_DOCS OFF CACHE BOOL "" FORCE)
    set(CATCH_INSTALL_HELPERS OFF CACHE BOOL "" FORCE)
    add_subdirectory(${catch2_SOURCE_DIR} ${catch2_BINARY_DIR} EXCLUDE_FROM_ALL)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
  endif()
  target_link_libraries(UtilExtern INTERFACE Catch2::Catch2)
endif(UTIL_BUILD_TESTS)

# ------------------------------------------------------------------------------
# GLFW (or SDL if prefered)

option(UTIL_PREFER_SDL_CONTEXT "Defines wether SDL is used for window & GL context creation." OFF)
if(UTIL_PREFER_SDL_CONTEXT)
  find_package(SDL2)
  if(SDL2_FOUND)
    target_compile_definitions(UtilExtern INTERFACE UTIL_PREFER_SDL_CONTEXT)
    target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_SDL2)
    target_include_directories(UtilExtern INTERFACE ${SDL2_INCLUDE_DIRS})
    target_link_libraries(UtilExtern INTERFACE ${SDL2_LIBRARIES})
  endif()
else()
  FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.3
  )

  FetchContent_GetProperties(glfw)
  if(NOT glfw_POPULATED)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "Fetching GLFW...")
    FetchContent_Populate(glfw)
    set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
    set(GLFW_INSTALL OFF CACHE BOOL "" FORCE)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
  endif()
  
	target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_GLFW)
	target_link_libraries(UtilExtern INTERFACE glfw)
endif()


# ------------------------------------------------------------------------------
# Network (asio)

# Dependency to SDL2_net
#find_package(SDL2_net 2.0.0)
#if(SDL2_NET_FOUND)
#	target_compile_definitions(Util PRIVATE UTIL_HAVE_LIB_SDL2_NET)
#	target_include_directories(Util PRIVATE ${SDL2_NET_INCLUDE_DIRS})
#	target_link_libraries(Util PRIVATE ${SDL2_NET_LIBRARIES})
#endif()

FetchContent_Declare(
  asio
  GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
  GIT_TAG        asio-1-13-0
)

FetchContent_GetProperties(asio)
if(NOT asio_POPULATED)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "Fetching ASIO...")
  FetchContent_Populate(asio)
  #add_subdirectory(${asio_SOURCE_DIR}/asio/include ${asio_BINARY_DIR} EXCLUDE_FROM_ALL)
  execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
endif()
target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_ASIO)
target_include_directories(UtilExtern INTERFACE ${asio_SOURCE_DIR}/asio/include)
#target_link_libraries(UtilExtern INTERFACE asio)
if(MINGW)
  target_link_libraries(UtilExtern INTERFACE ws2_32)
endif()

# ------------------------------------------------------------------------------
# CURL

option(UTIL_BUILD_CURL "Defines if curl should be build from source (requires further dependencies)." OFF)
find_package(CURL QUIET)
if(UTIL_BUILD_CURL)
  FetchContent_Declare(
    curl
    GIT_REPOSITORY https://github.com/curl/curl.git
    GIT_TAG        curl-7_65_3
  )

  FetchContent_GetProperties(curl)
  if(NOT curl_POPULATED)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo_append "Fetching CURL...")
    FetchContent_Populate(curl)
    set(BUILD_CURL_EXE OFF CACHE BOOL "" FORCE)
    if(WIN32)
      set(CMAKE_USE_WINSSL ON CACHE BOOL "" FORCE)
    endif()
    add_subdirectory(${curl_SOURCE_DIR} ${curl_BINARY_DIR} EXCLUDE_FROM_ALL)
    execute_process(COMMAND ${CMAKE_COMMAND} -E echo "Done")
  endif()
  target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_CURL)
  target_link_libraries(UtilExtern INTERFACE curl)
elseif(WIN32 AND MINGW)
  # TODO: Use prebuild binaries for MinGW on Windows
elseif(CURL_FOUND)
  target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_CURL)
  target_include_directories(UtilExtern INTERFACE ${CURL_INCLUDE_DIRS})
  target_link_libraries(UtilExtern INTERFACE ${CURL_LIBRARIES})
endif()