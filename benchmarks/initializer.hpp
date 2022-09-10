//  Use of this source code is governed by a BSD-style license that can
//  be found in the LICENSE file at the root of the source tree or at
//  <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_BENCHMARKS_INITIALIZER_HPP__
#define PV_BENCHMARKS_INITIALIZER_HPP__

#include "benchmark_classes.hpp"

#include <pv/polymorphic_variant.hpp>

#include <memory>
#include <variant>

pv::polymorphic_variant< Animal, Dog, Cat > initPolyVariant();
pv::polymorphic_variant< Animal, Dog, Cat > initPolyVariant(int arg);
std::variant< Dog, Cat > initStdVariant();
std::variant< Dog, Cat > initStdVariant(int arg);
std::unique_ptr< Animal > initRegular();
std::unique_ptr< Animal > initRegular(int arg);

template< typename T > struct initializer {};

template<> struct initializer< pv::polymorphic_variant< Animal, Dog, Cat > > {
	using storage_type = pv::polymorphic_variant< Animal, Dog, Cat >;

	static storage_type visibleInit() { return storage_type(Cat{}); }

	static storage_type hiddenInit() { return initPolyVariant(); }

	static storage_type visibleInit(int arg) { return storage_type(Cat(arg)); }

	static storage_type hiddenInit(int arg) { return initPolyVariant(arg); }
};

template<> struct initializer< std::variant< Dog, Cat > > {
	using storage_type = std::variant< Dog, Cat >;

	static storage_type visibleInit() { return storage_type(Cat{}); }

	static storage_type hiddenInit() { return initStdVariant(); }

	static storage_type visibleInit(int arg) { return storage_type(Cat(arg)); }

	static storage_type hiddenInit(int arg) { return initStdVariant(arg); }
};

template<> struct initializer< Animal > {
	using storage_type = std::unique_ptr< Animal >;

	static storage_type visibleInit() { return std::make_unique< Cat >(); }

	static storage_type hiddenInit() { return initRegular(); }

	static storage_type visibleInit(int arg) { return std::make_unique< Cat >(arg); }

	static storage_type hiddenInit(int arg) { return initRegular(arg); }
};

#endif // PV_BENCHMARKS_INITIALIZER_HPP__
