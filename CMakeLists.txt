# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

cmake_minimum_required(VERSION 3.15)

project(polymorphic_variant
	VERSION 1.0.0
	DESCRIPTION "A thin wrapper around a variant that makes it more suitable for use in closed-set polymorphism."
	LANGUAGES "CXX"
)

include(CTest)

add_library(polymorphic_variant INTERFACE)

target_include_directories(polymorphic_variant INTERFACE "${CMAKE_SOURCE_DIR}/include")
target_compile_features(polymorphic_variant INTERFACE cxx_std_17)

add_subdirectory(tests)
