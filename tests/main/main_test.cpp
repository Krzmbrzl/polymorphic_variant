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
	pv::polymorphic_variant<Base, Derived1, Base, Derived2> variant(Derived1{5});

	ASSERT_EQ(variant->the_value, 5);
}

TEST(main, assignable) {
	pv::polymorphic_variant<Base, Derived1, Base, Derived2> variant;

	ASSERT_EQ(variant->get_test(), Derived1::test_value);

	variant = Derived2();

	ASSERT_EQ(variant->get_test(), Derived2::test_value);

	variant.emplace<Base>(5);

	ASSERT_EQ(variant->get_test(), Base::test_value);
	ASSERT_EQ(variant->the_value, 5);
}
