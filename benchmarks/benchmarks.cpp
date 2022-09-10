//  Use of this source code is governed by a BSD-style license that can
//  be found in the LICENSE file at the root of the source tree or at
//  <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#include <benchmark/benchmark.h>

#include <pv/polymorphic_variant.hpp>

#include <algorithm>
#include <memory>
#include <random>
#include <vector>

#include "benchmark_classes.hpp"
#include "initializer.hpp"


template< typename T > static void BM_visibleInit(benchmark::State &state) {
	typename initializer< T >::storage_type value = initializer< T >::visibleInit();

	for (auto _ : state) {
		benchmark::DoNotOptimize(value->make_noise());
	}
}

BENCHMARK(BM_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >);
BENCHMARK(BM_visibleInit< Animal >);

template< typename T > static void BM_hiddenInit(benchmark::State &state) {
	typename initializer< T >::storage_type value = initializer< T >::hiddenInit();

	for (auto _ : state) {
		benchmark::DoNotOptimize(value->make_noise());
	}
}

BENCHMARK(BM_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >);
BENCHMARK(BM_hiddenInit< Animal >);

static void BM_devirtualized(benchmark::State &state) {
	Dog dog;
	Animal *animal = &dog;

	for (auto _ : state) {
		benchmark::DoNotOptimize(animal->make_noise());
	}
}

BENCHMARK(BM_devirtualized);

static void BM_classicalVariant_visibleInit(benchmark::State &state) {
	using T                                       = std::variant< Dog, Cat >;
	typename initializer< T >::storage_type value = initializer< T >::visibleInit();

	for (auto _ : state) {
		benchmark::DoNotOptimize(std::visit([](auto &&val) { return val.make_noise(); }, value));
	}
}

BENCHMARK(BM_classicalVariant_visibleInit);

static void BM_classicalVariant_hiddenInit(benchmark::State &state) {
	using T                                       = std::variant< Dog, Cat >;
	typename initializer< T >::storage_type value = initializer< T >::hiddenInit();

	for (auto _ : state) {
		benchmark::DoNotOptimize(std::visit([](auto &&val) { return val.make_noise(); }, value));
	}
}

BENCHMARK(BM_classicalVariant_hiddenInit);


template< typename T > static void BM_linearSearch_visibleInit(benchmark::State &state) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution< int > dist(-5, 5);

	std::vector< typename initializer< T >::storage_type > vec;
	vec.reserve(state.range(0));

	for (std::size_t i = 0; i < state.range(0); ++i) {
		vec.push_back(initializer< T >::visibleInit(dist(rng)));
	}

	for (auto _ : state) {
		benchmark::DoNotOptimize(
			// We search for an element that can't exist (due to the limits we chose for our RNG above)
			// to ensure that we iterate over the entire vector and don't stop early
			std::find_if(vec.begin(), vec.end(),
						 [](const typename initializer< T >::storage_type &val) { return val->get_member() > 10; }));
	}
}

BENCHMARK(BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >)->Range(1, std::pow(8, 6));
BENCHMARK(BM_linearSearch_visibleInit< Animal >)->Range(1, std::pow(8, 6));

template< typename T > static void BM_linearSearch_hiddenInit(benchmark::State &state) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution< int > dist(-5, 5);

	std::vector< typename initializer< T >::storage_type > vec;
	vec.reserve(state.range(0));

	for (std::size_t i = 0; i < state.range(0); ++i) {
		vec.push_back(initializer< T >::hiddenInit(dist(rng)));
	}

	for (auto _ : state) {
		benchmark::DoNotOptimize(
			// We search for an element that can't exist (due to the limits we chose for our RNG above)
			// to ensure that we iterate over the entire vector and don't stop early
			std::find_if(vec.begin(), vec.end(),
						 [](const typename initializer< T >::storage_type &val) { return val->get_member() > 10; }));
	}
}

BENCHMARK(BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >)->Range(1, std::pow(8, 6));
BENCHMARK(BM_linearSearch_hiddenInit< Animal >)->Range(1, std::pow(8, 6));

static void BM_classicalVariant_linearSearch_visibleInit(benchmark::State &state) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution< int > dist(-5, 5);

	using T = std::variant< Dog, Cat >;

	std::vector< typename initializer< T >::storage_type > vec;
	vec.reserve(state.range(0));

	for (std::size_t i = 0; i < state.range(0); ++i) {
		vec.push_back(initializer< T >::visibleInit(dist(rng)));
	}

	for (auto _ : state) {
		benchmark::DoNotOptimize(
			// We search for an element that can't exist (due to the limits we chose for our RNG above)
			// to ensure that we iterate over the entire vector and don't stop early
			std::find_if(vec.begin(), vec.end(), [](const typename initializer< T >::storage_type &val) {
				return std::visit([](auto &&v) { return v.get_member() > 10; }, val);
			}));
	}
}

BENCHMARK(BM_classicalVariant_linearSearch_visibleInit)->Range(1, std::pow(8, 6));

static void BM_classicalVariant_linearSearch_hiddenInit(benchmark::State &state) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution< int > dist(-5, 5);

	using T = std::variant< Dog, Cat >;

	std::vector< typename initializer< T >::storage_type > vec;
	vec.reserve(state.range(0));

	for (std::size_t i = 0; i < state.range(0); ++i) {
		vec.push_back(initializer< T >::hiddenInit(dist(rng)));
	}

	for (auto _ : state) {
		benchmark::DoNotOptimize(
			// We search for an element that can't exist (due to the limits we chose for our RNG above)
			// to ensure that we iterate over the entire vector and don't stop early
			std::find_if(vec.begin(), vec.end(), [](const typename initializer< T >::storage_type &val) {
				return std::visit([](auto &&v) { return v.get_member() > 10; }, val);
			}));
	}
}

BENCHMARK(BM_classicalVariant_linearSearch_hiddenInit)->Range(1, std::pow(8, 6));

static void BM_linearSearch_devirtualized(benchmark::State &state) {
	std::random_device dev;
	std::mt19937 rng(dev());
	std::uniform_int_distribution< int > dist(-5, 5);

	std::vector< Dog > vec;
	vec.reserve(state.range(0));

	for (std::size_t i = 0; i < state.range(0); ++i) {
		vec.push_back(Dog(dist(rng)));
	}

	for (auto _ : state) {
		benchmark::DoNotOptimize(
			// We search for an element that can't exist (due to the limits we chose for our RNG above)
			// to ensure that we iterate over the entire vector and don't stop early
			std::find_if(vec.begin(), vec.end(), [](const Dog &dog) { return dog.get_member() > 10; }));
	}
}

BENCHMARK(BM_linearSearch_devirtualized)->Range(1, std::pow(8, 6));

BENCHMARK_MAIN();
