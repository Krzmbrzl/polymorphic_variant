# polymorphic_variant

A thin wrapper around a variant that makes it more suitable for use in closed-set polymorphism.

## Motivation

If there exists some base class `Base` and a known set of subclasses (e.g. `Derived1` and `Derived2`), then we can use a `std::variant` to hold any of
these types without having to resort allocating memory in the free store (on the "heap") as `std::variant< Base, Derived1, Derived2 >`. This is known
as _closed-set polymorphism_ (all possible child-classes of `Base` are known at compile time).

While this gives us the benefit of value-semantics, it is a bit cumbersome, to actually access the currently stored object by virtue of the `Base`
interface. E.g. compare
```cpp
class Base {
public:
    void base_function();
};

std::variant< Base, Derived1, Derived2 > variant = ...;

std::visit([](auto &&v) { v.base_function(); }, variant);
```
with
```cpp
Base *base_ref = ...;

base_ref->base_function();
```


## Description

This mini, header-only library defines a new type `pv::polymorphic_variant` that for the most part acts as a `std::variant` (most of variant's
interface is available on `polymorphic_variant`) with the addition of the function `get`, and operators `->` and `*`, which all obtain a base-class 
reference (or pointer) instead of a reference (pointer) of the currently stored object type.

A `polymorphic_variant` takes the template arguments `polymorphic_variant< BaseClass, AssignableClasses >` where `AssignableClasses` is a non-empty
list of types that shall be stored inside the variant (may contain `BaseClass`, if `BaseClass` is not an abstract class. So for our example above, we
would use a `polymorphic_variant< Base, Base, Derived1, Derived2 >` and in order to access `Base::base_function`, we simply perform
```cpp
pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant;

variant->base_function();
```

Should you ever require access to the underlying `std::variant` object, you can access it via the `variant()` member function.


## Using variant's free functions

The standard C++ library defines several free functions that work on a `std::variant`. Unfortunately, all these functions do not work with
`pv::polymorphic_variant`, but there are overloads for all these functions inside the `pv` namespace, that should even allow to mix `std::variant`s
with `pv::polymorphic_variant`s.

```cpp
pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant;

pv::visit([](auto &&v) { v.base_function(); }, variant);
// Of course we now have a more direct way of accessing base_function,
// so the above example is only to illustrate how pv::visit can be used.
```


## Requirements

Only a C++17-compliant compiler is required, that fully supports `std::variant`.


## Usage

Simply include the `pv/polymorphic_variant.hpp` in your code and start using the `pv::polymorphic_variant` class right away.

### CMake

If you're using CMake, all you can either add this repo as a submodule and then use `add_subdirectory` to include this lib in your cmake build or you
can use `FetchContent` to download it on-the-fly:
```cmake
include(FetchContent)

FetchContent_Declare(polymorphic_variant
    GIT_REPOSITORY https://github.com/Krzmbrzl/polymorphic_variant
	GIT_TAG v1.0.0
)
FetchContent_MakeAvailable(polymorphic_variant)
```
In either case, the library will provide the `polymorphic_variant` target that you can simply link your target(s) to in order for the include-paths
and compile flags to be set accordingly.


## Building

The library itself does not require building as it is header-only. However, if you want to run the test cases, the procedure is as follows:
```bash
mkdir build
cd build
cmake -DPV_BUILD_TESTS=ON ..
cmake --build .

# Run the tests
ctest --output-on-failure
```

