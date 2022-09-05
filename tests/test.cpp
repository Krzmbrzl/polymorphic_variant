#include <pv/polymorphic_variant.hpp>
#include <pv/free_functions.hpp>

#include <iostream>

struct Base {
	int test = 0;

	virtual void setTest() = 0;
};

struct Derived1 : public Base {
	void setTest() override { test = 1; }
};

struct Derived2 : public Base {
	void setTest() override { test = 2; }
};

class Bla : public Base {
public:
	void setTest() override { test = 3; }
};

int main() {
	pv::polymorphic_variant< Base, Derived1, Derived2, Bla > variant(Derived2{});

	variant->setTest();

	std::cout << variant->test << " " << variant.index() << std::endl;

	pv::visit([](auto &&v) { std::cout << "Variant holds test value of " << v.test << std::endl; }, variant);
}
