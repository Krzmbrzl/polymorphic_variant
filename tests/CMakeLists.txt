# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

option(PV_BUILD_TESTS "Whether to build tests for the polymorphic_variant class" ${PROJECT_IS_TOP_LEVEL})

# Helper function for registering test-cases
function(register_test)
	set(options "")
	set(oneValueArgs "")
	set(multiValueArgs TARGETS)

	cmake_parse_arguments(LIBPERM_TEST "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if(LIBPERM_TEST_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "Unrecognized arguments: ${LIBPERM_TEST_UNPARSED_ARGUMENTS}")
	endif()

	foreach(CURRENT IN LISTS LIBPERM_TEST_TARGETS)
		if(NOT TARGET "${CURRENT}")
			message(FATAL_ERROR "\"${CURRENT} is not a target")
		endif()

		target_link_libraries("${CURRENT}" PRIVATE gmock gtest_main)
		gtest_discover_tests("${CURRENT}")
	endforeach()
endfunction()

if (PV_BUILD_TESTS)
	# Setup Google test
	include(FetchContent)
	FetchContent_Declare(
		GoogleTest
		GIT_REPOSITORY https://github.com/google/googletest
		GIT_TAG v1.12.0
	)
	FetchContent_MakeAvailable(GoogleTest)

	include(GoogleTest)

	include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

	add_subdirectory(main)
	add_subdirectory(free_functions)
	add_subdirectory(operators)
endif()
