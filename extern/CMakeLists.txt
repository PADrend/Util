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

include(CMakeDependentOption)

if(POLICY CMP0048)
  set(CMAKE_POLICY_DEFAULT_CMP0048 NEW)
endif()
if(POLICY CMP0063)
  set(CMAKE_POLICY_DEFAULT_CMP0063 NEW)
endif()
if(POLICY CMP0058)
  set(CMAKE_POLICY_DEFAULT_CMP0058 NEW)
endif()

# store state of BUILD_SHARED_LIBS to later restore
set(OLD_BUILD_SHARED_LIBS ${BUILD_SHARED_LIBS})
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)

# ------------------------------------------------------------------------------
# zlib (required by several other libraries)

FetchContent_Declare(
  zlib
  GIT_REPOSITORY https://github.com/madler/zlib.git
  GIT_TAG        v1.2.11
)

FetchContent_GetProperties(zlib)
if(NOT zlib_POPULATED)
  message(STATUS "Fetching zlib...")
  FetchContent_Populate(zlib)
  add_subdirectory(${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
target_link_libraries(UtilExtern INTERFACE zlibstatic)
set_target_properties(zlibstatic PROPERTIES POSITION_INDEPENDENT_CODE ON)

# Call find_package for zlib once and overwrite cache variables for other libraries
find_package(ZLIB QUIET)
set(ZLIB_FOUND TRUE CACHE INTERNAL "" FORCE)
set(ZLIB_INCLUDE_DIRS ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} CACHE INTERNAL "" FORCE)
set(ZLIB_INCLUDE_DIR ${zlib_SOURCE_DIR} ${zlib_BINARY_DIR} CACHE INTERNAL "" FORCE)
set(ZLIB_LIBRARY zlibstatic CACHE INTERNAL "" FORCE)
set(ZLIB_LIBRARIES zlibstatic CACHE INTERNAL "" FORCE)
set(ZLIB_LIBRARY_DEBUG zlibstatic CACHE INTERNAL "" FORCE)
set(ZLIB_LIBRARY_RELEASE zlibstatic CACHE INTERNAL "" FORCE)
set(ZLIB_VERSION_STRING "1.2.11" CACHE INTERNAL "" FORCE)

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
    message(STATUS "Fetching GLFW...")
    FetchContent_Populate(glfw)
    # hide options in cmake-gui
    set(GLFW_BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
    set(GLFW_BUILD_TESTS OFF CACHE INTERNAL "" FORCE)
    set(GLFW_BUILD_DOCS OFF CACHE INTERNAL "" FORCE)
    set(GLFW_INSTALL OFF CACHE INTERNAL "" FORCE)
    set(GLFW_VULKAN_STATIC OFF CACHE INTERNAL "" FORCE)
	set(USE_MSVC_RUNTIME_LIBRARY_DLL ON CACHE INTERNAL "" FORCE)
    add_subdirectory(${glfw_SOURCE_DIR} ${glfw_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  
	target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_GLFW)
	target_link_libraries(UtilExtern INTERFACE glfw)  
	set_target_properties(glfw PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# ------------------------------------------------------------------------------
# stb (multi-purpose single-file public domain libraries)

FetchContent_Declare(
  stb
  GIT_REPOSITORY https://github.com/nothings/stb.git
  GIT_TAG        052dce117ed989848a950308bd99eef55525dfb1
)

FetchContent_GetProperties(stb)
if(NOT stb_POPULATED)
  message(STATUS "Fetching stb...")
  FetchContent_Populate(stb)
endif()
target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_STB)
target_include_directories(UtilExtern INTERFACE ${stb_SOURCE_DIR})

# ------------------------------------------------------------------------------
# freetype (optional)

option(UTIL_PREFER_LIBFREETYPE "Use the FreeType library for rendering custom fonts (optional)" OFF)
if(UTIL_PREFER_LIBFREETYPE)
  find_package(Freetype)
  if(FREETYPE_FOUND)
    target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_FREETYPE)
    target_include_directories(UtilExtern INTERFACE ${FREETYPE_INCLUDE_DIRS})
    target_link_libraries(UtilExtern INTERFACE ${FREETYPE_LIBRARIES})
  endif()
endif()

# ------------------------------------------------------------------------------
# SDL_image (optional)

cmake_dependent_option(UTIL_PREFER_SDL_IMAGE "Defines wether SDL_image is used for image loading/storing (requires SDL and libpng)." OFF "UTIL_PREFER_SDL_CONTEXT" OFF)
if(UTIL_PREFER_SDL_IMAGE)
  # Dependency to libpng
  find_package(PNG)
  if(PNG_FOUND)
    target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_PNG)
    target_compile_definitions(UtilExtern INTERFACE "${PNG_DEFINITIONS}")
    target_include_directories(UtilExtern INTERFACE ${PNG_INCLUDE_DIR})
    target_link_libraries(UtilExtern INTERFACE ${PNG_LIBRARIES})
  endif()

  # Dependency to SDL2_image
  find_package(SDL2_image 2.0.0)
  if(SDL2_IMAGE_FOUND)
    target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_SDL2_IMAGE)
    target_include_directories(UtilExtern INTERFACE ${SDL2_IMAGE_INCLUDE_DIRS})
    target_link_libraries(UtilExtern INTERFACE ${SDL2_IMAGE_LIBRARIES})
  endif()
endif()

# ------------------------------------------------------------------------------
# Network (asio or SDL_net)

cmake_dependent_option(UTIL_PREFER_SDL_NET "Defines wether SDL_net is used for networking (requires SDL)." OFF "UTIL_PREFER_SDL_CONTEXT" OFF)
if(UTIL_PREFER_SDL_NET)
  find_package(SDL2_net 2.0.0)
  if(SDL2_NET_FOUND)
  	target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_SDL2_NET)
  	target_include_directories(UtilExtern INTERFACE ${SDL2_NET_INCLUDE_DIRS})
  	target_link_libraries(UtilExtern INTERFACE ${SDL2_NET_LIBRARIES})
  endif()
else()
  FetchContent_Declare(
    asio
    GIT_REPOSITORY https://github.com/chriskohlhoff/asio.git
    GIT_TAG        asio-1-13-0
  )

  FetchContent_GetProperties(asio)
  if(NOT asio_POPULATED)
    message(STATUS "Fetching ASIO...")
    FetchContent_Populate(asio)
  endif()
  target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_ASIO)
  target_include_directories(UtilExtern INTERFACE ${asio_SOURCE_DIR}/asio/include)
endif()

# ------------------------------------------------------------------------------
# CURL

option(UTIL_USE_CURL "Use the cURL library (optional; used for http/https/ftp file system support)." ON)
if(UTIL_USE_CURL)
  message(STATUS "Checking cURL...")
  find_package(CURL)
  if(CURL_FOUND)
    target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_CURL)
    target_include_directories(UtilExtern INTERFACE ${CURL_INCLUDE_DIRS})
    target_link_libraries(UtilExtern INTERFACE ${CURL_LIBRARIES})
  else()
    message(WARNING "Could not find cURL development libraries. If you want to have http/https/ftp file system support, please make sure it is installed on your system.")
  endif()
endif()

# ------------------------------------------------------------------------------
# OpenSSL (optional; used by some libraries)

option(UTIL_USE_OPENSSL "Use the OpenSSL library (optional; used by some libraries)." ON)
if(UTIL_USE_OPENSSL)
  message(STATUS "Checking OpenSSL...")
  find_package(OpenSSL)
  if(OPENSSL_FOUND)
    target_include_directories(UtilExtern INTERFACE ${OPENSSL_INCLUDE_DIR})
    target_link_libraries(UtilExtern INTERFACE ${OPENSSL_LIBRARIES})
  endif()
endif()

# ------------------------------------------------------------------------------
# libxml2 (optional)

option(UTIL_PREFER_LIBXML2 "Use the libxml2 library for XML parsing (optional)." ON)
if(UTIL_PREFER_LIBXML2)
  message(STATUS "Checking LibXml2...")
  find_package(LibXml2)
  if(LIBXML2_FOUND)
    target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_XML2)
    target_include_directories(UtilExtern INTERFACE ${LIBXML2_INCLUDE_DIR})
    target_link_libraries(UtilExtern INTERFACE ${LIBXML2_LIBRARIES})
  endif()
endif()

# ------------------------------------------------------------------------------
# LibArchive

option(UTIL_BUILD_LIBARCHIVE "Build the libarchive library (optional; used for Util's ArchiveProvider)." ON)
if(UTIL_BUILD_LIBARCHIVE)
  FetchContent_Declare(
    libarchive
    GIT_REPOSITORY https://github.com/libarchive/libarchive.git
    GIT_TAG        v3.4.3
  )

  FetchContent_GetProperties(libarchive)
  if(NOT libarchive_POPULATED)
    message(STATUS "Fetching libarchive...")
    FetchContent_Populate(libarchive)
    # hide options in cmake-gui
    set(ENABLE_WERROR OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_NETTLE OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_OPENSSL ${OPENSSL_FOUND} CACHE INTERNAL "" FORCE)
    set(ENABLE_LIBB2 OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_LZ4 OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_LZO OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_LZMA OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_ZSTD OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_ZLIB ON CACHE INTERNAL "" FORCE)
    set(ENABLE_BZip2 OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_LIBXML2 ${LIBXML2_FOUND} CACHE INTERNAL "" FORCE)
    set(ENABLE_EXPAT OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_PCREPOSIX OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_LibGCC OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_CNG OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_TAR OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_TAR_SHARED FALSE CACHE INTERNAL "" FORCE)
    set(ENABLE_CPIO OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_CPIO_SHARED FALSE CACHE INTERNAL "" FORCE)
    set(ENABLE_CAT OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_CAT_SHARED FALSE CACHE INTERNAL "" FORCE)
    set(ENABLE_XATTR OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_ACL OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_ICONV OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_TEST OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_COVERAGE FALSE CACHE INTERNAL "" FORCE)
    set(ENABLE_INSTALL OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${libarchive_SOURCE_DIR} ${libarchive_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_ARCHIVE)
  target_compile_definitions(UtilExtern INTERFACE LIBARCHIVE_STATIC)
  target_link_libraries(UtilExtern INTERFACE archive_static)
  set_target_properties(archive_static PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_include_directories(UtilExtern INTERFACE ${libarchive_SOURCE_DIR}/libarchive)
endif()

# ------------------------------------------------------------------------------
# libzip

option(UTIL_BUILD_LIBZIP "Build the libzip library (optional; used for Util's ZIPProvider)." ON)
if(UTIL_BUILD_LIBZIP)
  FetchContent_Declare(
    libzip
    GIT_REPOSITORY https://github.com/nih-at/libzip.git
    GIT_TAG        rel-1-6-0
  )
  FetchContent_GetProperties(libzip)
  if(NOT libzip_POPULATED)
    message(STATUS "Fetching libzip...")
    FetchContent_Populate(libzip)
    # hide options in cmake-gui
    set(ENABLE_COMMONCRYPTO ON CACHE INTERNAL "" FORCE)
    set(ENABLE_GNUTLS OFF CACHE INTERNAL "" FORCE)
    set(ENABLE_MBEDTLS ON CACHE INTERNAL "" FORCE)
    set(ENABLE_OPENSSL ${OPENSSL_FOUND} CACHE INTERNAL "" FORCE)
    set(ENABLE_WINDOWS_CRYPTO ON CACHE INTERNAL "" FORCE)
    set(ENABLE_BZIP2 OFF CACHE INTERNAL "" FORCE)
    set(BUILD_TOOLS OFF CACHE INTERNAL "" FORCE)
    set(BUILD_REGRESS OFF CACHE INTERNAL "" FORCE)
    set(BUILD_EXAMPLES OFF CACHE INTERNAL "" FORCE)
    set(BUILD_DOC OFF CACHE INTERNAL "" FORCE)
    set(SHARED_LIB_VERSIONNING OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${libzip_SOURCE_DIR} ${libzip_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_ZIP)
  target_link_libraries(UtilExtern INTERFACE zip)
  target_include_directories(UtilExtern INTERFACE ${libzip_SOURCE_DIR}/lib ${libzip_BINARY_DIR})
  set_target_properties(zip PROPERTIES POSITION_INDEPENDENT_CODE ON)
endif()

# ------------------------------------------------------------------------------
# sqlite3

option(UTIL_BUILD_SQLITE "Build the SQLite library (recommended; used for Util's DBFSProvider)." ON)
if(UTIL_BUILD_SQLITE)
  FetchContent_Declare(
    sqlite3
    GIT_REPOSITORY https://github.com/azadkuh/sqlite-amalgamation.git
    GIT_TAG        3.29.0
  )

  FetchContent_GetProperties(sqlite3)
  if(NOT sqlite3_POPULATED)
    message(STATUS "Fetching sqlite3...")
    FetchContent_Populate(sqlite3)
    # hide options in cmake-gui
    set(BUILD_ENABLE_DBSTAT_VTAB OFF CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_FTS3 OFF CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_FTS4 OFF CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_FTS5 OFF CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_GEOPOLY OFF CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_ICU OFF CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_JSON1 ON CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_RBU OFF CACHE INTERNAL "" FORCE)
    set(BUILD_ENABLE_RTREE OFF CACHE INTERNAL "" FORCE)
    set(BUILD_RECOMMENDED_OPTS ON CACHE INTERNAL "" FORCE)
    set(BUILD_STATIC_RUNTIME OFF CACHE INTERNAL "" FORCE)
    set(BUILD_SHELL OFF CACHE INTERNAL "" FORCE)
    add_subdirectory(${sqlite3_SOURCE_DIR} ${sqlite3_BINARY_DIR} EXCLUDE_FROM_ALL)
  endif()
  set_target_properties(sqlite3 PROPERTIES POSITION_INDEPENDENT_CODE ON)
  target_compile_definitions(UtilExtern INTERFACE UTIL_HAVE_LIB_SQLITE)
  target_link_libraries(UtilExtern INTERFACE sqlite3)
  target_include_directories(UtilExtern INTERFACE ${sqlite3_SOURCE_DIR})
endif()

# ------------------------------------------------------------------------------
# rang (console/terminal colors)

FetchContent_Declare(
  rang
  GIT_REPOSITORY https://github.com/agauniyal/rang.git
  GIT_TAG        v3.2
)

FetchContent_GetProperties(rang)
if(NOT rang_POPULATED)
  message(STATUS "Fetching rang...")
  FetchContent_Populate(rang)
  add_subdirectory(${rang_SOURCE_DIR} ${rang_BINARY_DIR} EXCLUDE_FROM_ALL)
endif()
target_link_libraries(UtilExtern INTERFACE rang)

# ------------------------------------------------------------------------------

if(MINGW)
  target_link_libraries(UtilExtern INTERFACE ws2_32 crypt32 wldap32)
endif()

set(BUILD_SHARED_LIBS ${OLD_BUILD_SHARED_LIBS} CACHE INTERNAL "" FORCE)