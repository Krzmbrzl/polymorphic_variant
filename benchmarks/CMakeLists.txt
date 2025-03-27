# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

option(PV_BUILD_BENCHMARKS "Whether to build benchmarks for the polymorphic_variant class" ${PROJECT_IS_TOP_LEVEL})

if (PV_BUILD_BENCHMARKS)
	include(FetchContent)

	set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
	FetchContent_Declare(
		GoogleBenchmark
		GIT_REPOSITORY https://github.com/google/benchmark.git
		GIT_TAG v1.9.1
		EXCLUDE_FROM_ALL
	)
	FetchContent_MakeAvailable(GoogleBenchmark)

	add_executable(polymorphic_variant_benchmark
		"benchmarks.cpp"
		"initializer.cpp"
	)

	target_link_libraries(polymorphic_variant_benchmark PRIVATE benchmark::benchmark polymorphic_variant)
	set_internal_build_flags(polymorphic_variant_benchmark)
endif()
