# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

cmake_minimum_required(VERSION 3.23)

project(polymorphic_variant
	VERSION 1.5.0
	DESCRIPTION "A thin wrapper around a variant that makes it more suitable for use in closed-set polymorphism."
	LANGUAGES "CXX"
)

if (NOT DEFINED CMAKE_CXX_STANDARD OR CMAKE_CXX_STANDARD LESS 17)
	set(CMAKE_CXX_STANDARD 17)
endif()
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

include(CTest)
include(dependencies)
include(variant_uses_shared_storage)
include(GNUInstallDirs)
include(CMakePackageConfigHelpers)

variant_uses_shared_storage(RESULT_VAR USES_SHARED_STORAGE)

message(STATUS "std::variant uses shared storage: ${USES_SHARED_STORAGE}")

option(
	PV_EXPLOIT_SHARED_STORAGE
	"Whether to exploit the fact that std::variant uses a shared storage area for all alternatives"
	${USES_SHARED_STORAGE}
)

message(STATUS "PV_EXPLOIT_SHARED_STORAGE: ${PV_EXPLOIT_SHARED_STORAGE}")


add_library(polymorphic_variant INTERFACE)
add_library(polymorphic_variant::polymorphic_variant ALIAS polymorphic_variant)

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

file(GLOB_RECURSE PV_HEADER_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/include/pv/*.hpp")
target_sources(polymorphic_variant
PUBLIC
	FILE_SET headers TYPE HEADERS
	BASE_DIRS "${PROJECT_SOURCE_DIR}/include"
	FILES "${PV_HEADER_FILES}"
)
target_include_directories(polymorphic_variant SYSTEM INTERFACE "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>")
target_compile_features(polymorphic_variant INTERFACE cxx_std_17)


add_subdirectory(tests)
add_subdirectory(benchmarks)


set(PV_INSTALL_CMAKEPATH "${CMAKE_INSTALL_LIBDIR}/cmake/polymorphic_variant-${PROJECT_VERSION_MAJOR}")

install(
	TARGETS polymorphic_variant
	EXPORT polymorphic_variant-targets
	RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}"
		COMPONENT polymorphic_variant_runtime
	LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		COMPONENT polymorphic_variant_runtime
		NAMELINK_COMPONENT polymorphic_variant_development
	ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
		COMPONENT polymorphic_variant_development
	FILE_SET headers
)
install(
	EXPORT polymorphic_variant-targets
	DESTINATION "${PV_INSTALL_CMAKEPATH}"
	FILE polymorphic_variant-targets.cmake
	NAMESPACE polymorphic_variant::
	COMPONENT polymorphic_variant_development
)

configure_package_config_file(
	"${PROJECT_SOURCE_DIR}/cmake/polymorphic_variant-config.cmake.in" "${CMAKE_CURRENT_BINARY_DIR}/polymorphic_variant-config.cmake"
	INSTALL_DESTINATION "${PV_INSTALL_CMAKEPATH}"
)
write_basic_package_version_file(
	"${CMAKE_BINARY_DIR}/polymorphic_variant-config-version.cmake"
	VERSION ${PROJECT_VERSION}
	COMPATIBILITY SameMajorVersion
)
install(FILES
	"${CMAKE_CURRENT_BINARY_DIR}/polymorphic_variant-config.cmake"
	"${CMAKE_CURRENT_BINARY_DIR}/polymorphic_variant-config-version.cmake"
	DESTINATION "${PV_INSTALL_CMAKEPATH}"
)


