// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_TESTS_TESTDEFINITIONS_HPP_
#define PV_TESTS_TESTDEFINITIONS_HPP_

class Base {
public:
	static constexpr int test_value = 0;

	int the_value = 0;

	Base() = default;
	Base(int i) : the_value(i) {}

	virtual ~Base() = default;

	virtual int get_test() const { return test_value; }

	bool operator==(Base &other) {
		return the_value == other.the_value;
	}
};

class Derived1 : public Base {
public:
	static constexpr int test_value = 1;

	static constexpr int field_value = 18;
	int derived1Field = field_value;

	using Base::Base;

	virtual int get_test() const override { return test_value; }
};

class Derived2 : public Base {
public:
	static constexpr int test_value = 2;

	static constexpr int field_value = 42;
	int derived2Field = field_value;

	using Base::Base;

	virtual int get_test() const override { return test_value; }
};

#endif // PV_TESTS_TESTDEFINITIONS_HPP_
