# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

cmake_minimum_required(VERSION 3.15)

project(polymorphic_variant
	VERSION 1.0.0
	DESCRIPTION "A thin wrapper around a variant that makes it more suitable for use in closed-set polymorphism."
	LANGUAGES "CXX"
)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

include(CTest)
include(dependencies)
include(variant_uses_shared_storage)

variant_uses_shared_storage(RESULT_VAR USES_SHARED_STORAGE)

message(STATUS "std::variant uses shared storage: ${USES_SHARED_STORAGE}")

option(
	PV_EXPLOIT_SHARED_STORAGE
	"Whether to exploit the fact that std::variant uses a shared storage area for all alternatives"
	${USES_SHARED_STORAGE}
)

message(STATUS "PV_EXPLOIT_SHARED_STORAGE: ${PV_EXPLOIT_SHARED_STORAGE}")


add_library(polymorphic_variant INTERFACE)

include(compiler)

if (PV_EXPLOIT_SHARED_STORAGE)
	target_compile_definitions(polymorphic_variant INTERFACE "PV_USE_SHARED_VARIANT_STORAGE")
endif()

# See https://stackoverflow.com/q/74181468
if (CMAKE_SOURCE_DIR STREQUAL PROJECT_SOURCE_DIR)
	# Project is processed as stand-alone (in contrast to being embedded in another project via
	# e.g. add_subdirectory)
	set(SYSTEM_SPEC "")
else()
	# Declare include dir as "system" to avoid compiler warnings for our code appearing
	# to downstream users.
	set(SYSTEM_SPEC "SYSTEM")
endif()

target_include_directories(polymorphic_variant ${SYSTEM_SPEC} INTERFACE "${PROJECT_SOURCE_DIR}/include")
target_compile_features(polymorphic_variant INTERFACE cxx_std_17)

add_subdirectory(tests)
add_subdirectory(benchmarks)
