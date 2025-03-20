// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#include <pv/polymorphic_variant.hpp>

#include <gtest/gtest.h>
#include <test_definitions.hpp>

#include <algorithm>

TEST(free_functions, visit) {
	pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant(Derived1{ 4 });

	int test_value = pv::visit([](auto &&v) { return v.get_test(); }, variant);
	int the_value  = pv::visit([](auto &&v) { return v.the_value; }, variant);

	ASSERT_EQ(test_value, Derived1::test_value);
	ASSERT_EQ(the_value, 4);
}

TEST(free_functions, holds_alternative) {
	pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant(Derived1{ 4 });

	ASSERT_FALSE(pv::holds_alternative< Base >(variant));
	ASSERT_TRUE(pv::holds_alternative< Derived1 >(variant));
	ASSERT_FALSE(pv::holds_alternative< Derived2 >(variant));
}

TEST(free_functions, get) {
	pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant(Derived1{ 4 });

	ASSERT_THROW(pv::get< 0 >(variant), std::bad_variant_access);
	ASSERT_THROW(pv::get< Base >(variant), std::bad_variant_access);

	ASSERT_EQ(pv::get< 1 >(variant).get_test(), Derived1::test_value);
	ASSERT_EQ(pv::get< Derived1 >(variant).get_test(), Derived1::test_value);
}

TEST(free_functions, get_if) {
	pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant(Derived1{ 4 });

	ASSERT_EQ(pv::get_if< 0 >(&variant), nullptr);
	ASSERT_EQ(pv::get_if< Base >(&variant), nullptr);

	ASSERT_TRUE(pv::get_if< 1 >(&variant));
	ASSERT_TRUE(pv::get_if< Derived1 >(&variant));

	ASSERT_EQ(pv::get_if< Derived1 >(&variant)->get_test(), Derived1::test_value);

	Base *result = pv::get_if< Base, decltype(variant) >(nullptr);
	ASSERT_EQ(result, nullptr);

	result = pv::get_if< Derived1, decltype(variant) >(nullptr);
	ASSERT_EQ(result, nullptr);
}

bool operator==(const Base &lhs, const Base &rhs) {
	return lhs.get_test() == rhs.get_test() && lhs.the_value == rhs.the_value;
}

bool operator!=(const Base &lhs, const Base &rhs) {
	return !(lhs == rhs);
}

TEST(free_functions, compare) {
	pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant1(Base{ 4 });
	pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant2(Derived2{ 7 });

	ASSERT_EQ(variant1, variant1);
	ASSERT_EQ(variant1, variant1.get());
	ASSERT_EQ(variant1.get(), variant1);
	ASSERT_NE(variant2, variant1);
	ASSERT_NE(variant2.get(), variant1);
	ASSERT_NE(variant2, variant1.get());
}

struct Number {
	virtual float getValue() const = 0;
};
struct Int : Number {
	Int(int val) : value(val) {}
	int value = 0;
	float getValue() const override { return static_cast< float >(value); }
};
struct Float : Number {
	Float(float val) : value(val) {}
	float value = 0;
	float getValue() const override { return value; }
};

bool operator==(const Number &lhs, const Number &rhs) {
	return lhs.getValue() == rhs.getValue();
}
bool operator==(const Number &lhs, int rhs) {
	return lhs.getValue() == static_cast< float >(rhs);
}
bool operator==(int lhs, const Number &rhs) {
	return static_cast< float >(lhs) == rhs.getValue();
}
bool operator<(const Number &lhs, const Number &rhs) {
	return lhs.getValue() < rhs.getValue();
}

TEST(free_functions, stl_integration) {
	using poly_number = pv::polymorphic_variant< Number, Int, Float >;

	std::vector< poly_number > firstList  = { poly_number(Int{ 3 }), poly_number(Int{ 2 }), poly_number(Float{ 5 }) };
	std::vector< poly_number > secondList = { poly_number(Float{ 2 }), poly_number(Int{ 5 }), poly_number(Float{ 3 }) };
	std::vector< int > nativeList         = { 2, 3, 5 };

	ASSERT_FALSE(std::equal(firstList.begin(), firstList.end(), secondList.begin(), secondList.end()));
	ASSERT_FALSE(std::equal(firstList.begin(), firstList.end(), nativeList.begin(), nativeList.end()));

	std::sort(firstList.begin(), firstList.end());
	std::sort(secondList.begin(), secondList.end());

	ASSERT_TRUE(std::equal(firstList.begin(), firstList.end(), secondList.begin(), secondList.end()));
	ASSERT_TRUE(std::equal(firstList.begin(), firstList.end(), nativeList.begin(), nativeList.end()));

	std::reverse(secondList.begin(), secondList.end());

	ASSERT_FALSE(std::equal(firstList.begin(), firstList.end(), secondList.begin(), secondList.end()));

	ASSERT_TRUE(std::is_permutation(firstList.begin(), firstList.end(), secondList.begin()));
	ASSERT_TRUE(std::is_permutation(firstList.begin(), firstList.end(), nativeList.begin()));
}
