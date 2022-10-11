// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#include <pv/polymorphic_variant.hpp>

#include <gtest/gtest.h>
#include <test_definitions.hpp>

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
