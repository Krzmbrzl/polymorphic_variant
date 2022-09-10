//  Use of this source code is governed by a BSD-style license that can
//  be found in the LICENSE file at the root of the source tree or at
//  <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_BENCHMARKS_BENCHMARK_CLASSES_HPP__
#define PV_BENCHMARKS_BENCHMARK_CLASSES_HPP__

#include <string>
#include <array>

class Animal {
public:
	Animal() = default;

	std::array<int, 100> filler;

	virtual std::string make_noise() const = 0;
	virtual int get_member() const {
		return 1;
	}
};

class Dog : public Animal {
public:
	int member = 0;
	Dog() = default;
	Dog(int i) : member(i) {}

	std::string make_noise() const override { return "bark"; }

	int get_member() const override { return member; }
};

class Cat : public Animal {
public:
	int member = 0;
	Cat() = default;
	Cat(int i) : member(i) {}

	std::string make_noise() const override { return "miau"; }

	int get_member() const override { return member; }
};

#endif // PV_BENCHMARKS_BENCHMARK_CLASSES_HPP__
