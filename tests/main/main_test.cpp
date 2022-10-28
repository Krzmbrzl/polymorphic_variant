// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#include <pv/polymorphic_variant.hpp>

#include <gtest/gtest.h>
#include <test_definitions.hpp>


TEST(main, default_constructible) {
	pv::polymorphic_variant< Base, Base > variant1;

	ASSERT_EQ(variant1->get_test(), Base::test_value);

	pv::polymorphic_variant< Base, Derived1, Derived2 > variant2;

	ASSERT_EQ(variant2->get_test(), Derived1::test_value);
}

TEST(main, construct_from_value) {
	pv::polymorphic_variant< Base, Derived1, Base, Derived2 > variant(Derived1{ 5 });

	ASSERT_EQ(variant->the_value, 5);
}

TEST(main, assignable) {
	pv::polymorphic_variant< Base, Derived1, Base, Derived2 > variant;

	ASSERT_EQ(variant->get_test(), Derived1::test_value);

	variant = Derived2();

	ASSERT_EQ(variant->get_test(), Derived2::test_value);

	variant.emplace< Base >(5);

	ASSERT_EQ(variant->get_test(), Base::test_value);
	ASSERT_EQ(variant->the_value, 5);
}

TEST(main, get) {
	pv::polymorphic_variant< Base, Derived1, Base, Derived2 > variant(Derived1{});

	{
		Derived1 &derivedRef = dynamic_cast< Derived1 & >(variant.get());

		ASSERT_EQ(derivedRef.derived1Field, Derived1::field_value);
	}

	variant = Derived2{};

	{
		Derived2 &derivedRef = dynamic_cast< Derived2 & >(variant.get());

		ASSERT_EQ(derivedRef.derived2Field, Derived2::field_value);
	}
}

TEST(main, copy) {
	// Test that each variant will refer to its own data (instead of ending up sharing data, due to some messed up
	// base-pointer logic)
	pv::polymorphic_variant< Base, Derived1, Base, Derived2 > variant1(Derived1{});

	ASSERT_EQ(variant1->get_test(), Derived1::test_value);

	pv::polymorphic_variant< Base, Derived1, Base, Derived2 > variant2 = variant1;
	variant1                                                           = Derived2{};

	ASSERT_EQ(variant1->get_test(), Derived2::test_value);
	ASSERT_EQ(variant2->get_test(), Derived1::test_value);
}

TEST(main, move) {
	// Test that each variant will refer to its own data (instead of ending up sharing data, due to some messed up
	// base-pointer logic)
	pv::polymorphic_variant< Base, Derived1, Base, Derived2 > variant1(Derived1{});

	ASSERT_EQ(variant1->get_test(), Derived1::test_value);

	pv::polymorphic_variant< Base, Derived1, Base, Derived2 > variant2 = std::move(variant1);
	variant1                                                           = Derived2{};

	ASSERT_EQ(variant1->get_test(), Derived2::test_value);
	ASSERT_EQ(variant2->get_test(), Derived1::test_value);
}
