// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#include <pv/polymorphic_variant.hpp>

#include <gtest/gtest.h>

#include "operator_classes.hpp"

#include <algorithm>

using variant_type = pv::polymorphic_variant< Base, Derived1, Derived2 >;


bool operator==(const Base &lhs, const Base &rhs) {
	return lhs.result() == rhs.result() && lhs.factor() == rhs.factor();
}

bool operator!=(const Base &lhs, const Base &rhs) {
	return !(lhs == rhs);
}

TEST(operators, compare) {
	variant_type variant1(Derived1{});
	variant_type variant2(Derived2{});

	ASSERT_EQ(variant1, variant1);
	ASSERT_EQ(variant1, variant1.get());
	ASSERT_EQ(variant1.get(), variant1);
	ASSERT_NE(variant2, variant1);
	ASSERT_NE(variant2.get(), variant1);
	ASSERT_NE(variant2, variant1.get());
}

TEST(operators, multiply) {
	// Operator implemented as member function
	variant_type variant1(Derived1{});
	variant_type variant2(Derived2{});

	int result = variant1 * variant2;

	ASSERT_EQ(result, Derived1::InitialValue * Derived2::Factor);
}

TEST(operators, divide) {
	// Operator implemented as free function
	variant_type variant1(Derived1{});
	variant_type variant2(Derived2{});

	double result = variant1 / variant2;

	ASSERT_EQ(result, Derived1::InitialValue / static_cast< double >(Derived2::Factor));
}

TEST(operators, multiply_assign) {
	// This operator mutates its lhs and therefore requires a non-const overload for the operator impl
	variant_type variant1(Derived1{});
	variant_type variant2(Derived2{});

	variant1 *= variant2;

	ASSERT_EQ(variant1->result(), Derived1::InitialValue * Derived2::Factor);
}

TEST(operators, add_assign) {
	// This operator mutates its lhs and therefore requires a non-const overload for the operator impl
	variant_type variant1(Derived1{});
	variant_type variant2(Derived2{});

	variant1 += variant2;

	ASSERT_EQ(variant1->result(), Derived1::InitialValue + Derived2::InitialValue);
}

TEST(operators, prefix_increment) {
	variant_type variant(Derived1{});

	int initial = variant->result();
	++variant;

	ASSERT_EQ(variant->result(), initial + 1);
}

TEST(operators, subscript) {
	const variant_type variant1(Derived1{});
	variant_type variant2(Derived1{});

	ASSERT_EQ(variant1[5], 5);
	ASSERT_EQ(variant2[5], 5);
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
