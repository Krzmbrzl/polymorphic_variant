# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

# Function to check whether the used std::variant implementation uses
# a common storage location for all its value types
function(variant_uses_shared_storage)
	set(options)
	set(oneValueArgs RESULT_VAR)
	set(multiValueArgs)
	cmake_parse_arguments(VARIANT_SHARED "${options}" "${oneValueArgs}" "${multiValueArgs}" ${ARGN})

	if (VARIANT_SHARED_UNPARSED_ARGUMENTS)
		message(FATAL_ERROR "variant_uses_shared_storage: Unrecognized arguments: ${VARIANT_SHARED_UNPARSED_ARGUMENTS}")
	endif()

	try_run(RUN_RESULT COMPILE_RESULT "${CMAKE_CURRENT_BINARY_DIR}" "${CMAKE_SOURCE_DIR}/cmake/snippets/check_variant_uses_shared_storage.cpp")

	if (NOT COMPILE_RESULT)
		message(FATAL_ERROR "variant_uses_shared_storage: Failed to compile code snippet")
	endif()

	if (RUN_RESULT EQUAL 0)
		set(${VARIANT_SHARED_RESULT_VAR} TRUE PARENT_SCOPE)
	else()
		set(${VARIANT_SHARED_RESULT_VAR} FALSE PARENT_SCOPE)
	endif()
endfunction()
