# Try to find shaderc. Once done, this will define:
#
#   SHADERC_FOUND - variable which returns the result of the search
#   SHADERC_INCLUDE_DIRS - list of include directories
#   SHADERC_LIBRARIES - options for the linker

#=============================================================================
#
# This work is licensed under the Creative Commons Attribution-ShareAlike 4.0 International License.
# To view a copy of this license, visit http://creativecommons.org/licenses/by-sa/4.0/.
# Author: Sascha Brandt (sascha@brandt.graphics)
#
#=============================================================================


# Find shaderc include dir
find_path(SHADERC_INCLUDE_DIR
	shaderc/shaderc.h
	PATHS
	${CMAKE_CURRENT_SOURCE_DIR}/../include
	${FETCHCONTENT_BASE_DIR}/shaderc-src/libshaderc/include
	/upb/groups/fg-madh/public/share/padrend/include
	$ENV{VULKAN_SDK}/include
)

# Find shaderc
if(TARGET shaderc_combined)
	# The library is included as a build target
	message("Found target shaderc_combined")
	set(SHADERC_LIBRARY shaderc_combined)
else()
	find_library(SHADERC_LIBRARY
		shaderc_combined
		PATHS
		${FETCHCONTENT_BASE_DIR}/shaderc-build/libshaderc
		${CMAKE_CURRENT_SOURCE_DIR}/../bin
		${CMAKE_CURRENT_SOURCE_DIR}/../lib
		${CMAKE_CURRENT_SOURCE_DIR}/../build
		${CMAKE_CURRENT_SOURCE_DIR}/../../build
		/upb/groups/fg-madh/public/share/padrend/linux/lib
		$ENV{VULKAN_SDK}/Lib
	)
endif()

set(SHADERC_INCLUDE_DIRS ${SHADERC_INCLUDE_DIR})
set(SHADERC_LIBRARIES ${SHADERC_LIBRARY})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(shaderc DEFAULT_MSG
	SHADERC_INCLUDE_DIR
	SHADERC_LIBRARY
)

mark_as_advanced(
	SHADERC_INCLUDE_DIR
	SHADERC_LIBRARY
)
