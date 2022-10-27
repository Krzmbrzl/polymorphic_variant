// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#include <pv/polymorphic_variant.hpp>

#include <gtest/gtest.h>

#include "operator_classes.hpp"

using variant_type = pv::polymorphic_variant< Base, Derived1, Derived2 >;


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

TEST(operators, prefix_increment) {
	variant_type variant(Derived1{});

	int initial = variant->result();
	++variant;

	ASSERT_EQ(variant->result(), initial + 1);
}
