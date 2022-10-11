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
interface is available on `polymorphic_variant`) with the addition of the function `get`, and operators `->` and unary `*`, which all obtain a
base-class reference (or pointer) instead of a reference (pointer) of the currently stored object type. Additionally, the `polymorphic_variant` will
expose almost all operators defined for the base-class (except those mentioned above, which are already used by the variant's own interface).

A `polymorphic_variant` takes the template arguments `polymorphic_variant< BaseClass, AssignableClasses >` where `AssignableClasses` is a non-empty
list of types that shall be stored inside the variant (may contain `BaseClass`, if `BaseClass` is not an abstract class. So for our example above, we
would use a `polymorphic_variant< Base, Base, Derived1, Derived2 >` and in order to access `Base::base_function`, we simply perform
```cpp
pv::polymorphic_variant< Base, Base, Derived1, Derived2 > variant;

variant->base_function();
```
and in order to access the base-class's `operator==` (assuming it has one), all that is needed is
```cpp
variant == ...
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

Note: AppleClang v12 has a bug (sometimes) preventing the operator overloads to be disabled, if inappropriate. However, this version is only shipped
on macOS 10.15, which is (almost) EOL and shouldn't be used anymore. AppleClang v13 (and presumably higher) work as expected.


## Usage

Simply include the `pv/polymorphic_variant.hpp` in your code and start using the `pv::polymorphic_variant` class right away.

### CMake

If you're using CMake, all you can either add this repo as a submodule and then use `add_subdirectory` to include this lib in your cmake build or you
can use `FetchContent` to download it on-the-fly:
```cmake
include(FetchContent)

FetchContent_Declare(polymorphic_variant
    GIT_REPOSITORY https://github.com/Krzmbrzl/polymorphic_variant
	GIT_TAG v1.2.0
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


## Performance

Based on the [benchmarks](./benchmarks) I have performed on my system (Linux, GCC 9.4.0) it seems that accessing a base-class function via
`polymorphic_variant` is about 40% faster than accessing it via a `std::unique_ptr`. However, in cases in which the compiler can devirtualize the call,
because it knows the actual type of the accessed object, accessing via `unique_ptr` is about twice as fast as via `polymorphic_variant` because at
least for me no devirtualization happened when accessing the object via `polymorphic_variant`.

Compared to using a traditional `std::variant`, using `polymorphic_variant` is always faster (about 20%).

In a test case of searching a `std::vector` of objects for an element that yields a particular value by calling a base-class method, using
`unique_ptr` seems to yield slightly better performance than `polymorphic_variant` for smaller vector sizes (up to about 512 elements, for which using
`unique_ptr` is about 2% faster) but for larger vector sizes, the picture changes drastically and `polymorphic_variant` becomes much more efficient
(up to 260% faster). In these cases, even using `std::variant` is a lot faster than `unique_ptr`, though `std::variant` is still about 40% slower than
using `polymorphic_variant`.

The drastic difference for larger vector sizes is most likely due to cache effects, where the variant-based approaches have the advantage that the
objects are stored directly in the vector so that while traversing the vector, the objects (whose properties need to be accessed) will end up in
cache. The `unique_ptr` approach on the other hand only stores pointers and thus have to fetch the objects at an arbitrary memory location, which
makes it unlikely that the accessed object is in cache.

If you want to build and check the benchmarks yourself, use `-DPV_BUILD_BENCHMARKS=ON` when invoking cmake.

<details>
	<summary>Benchmark results</summary>

```
2022-09-12T09:51:02+02:00
Running ./benchmarks/polymorphic_variant_benchmark
Run on (8 X 3487.21 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 0.61, 0.60, 0.87
----------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                  Time             CPU   Iterations
----------------------------------------------------------------------------------------------------------------------------
BM_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >                           2.06 ns         2.06 ns    339474210
BM_visibleInit< Animal >                                                                1.03 ns         1.03 ns    674996555
BM_visibleInit< std::variant< Dog, Cat > >                                              2.58 ns         2.58 ns    271188673
BM_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >                            1.81 ns         1.81 ns    386446207
BM_hiddenInit< Animal >                                                                 2.58 ns         2.58 ns    271375661
BM_hiddenInit< std::variant< Dog, Cat > >                                               2.58 ns         2.58 ns    271513655
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/1            3.86 ns         3.86 ns    180963759
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/8            14.0 ns         14.0 ns     50026633
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/64           93.7 ns         93.7 ns      7430824
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/512           701 ns          701 ns       992938
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/4096         7877 ns         7877 ns        88412
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/32768       75677 ns        75665 ns         9192
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/262144    1536286 ns      1535973 ns          461
BM_linearSearch_visibleInit< Animal >/1                                                 3.86 ns         3.86 ns    181001013
BM_linearSearch_visibleInit< Animal >/8                                                 13.3 ns         13.3 ns     52401587
BM_linearSearch_visibleInit< Animal >/64                                                89.2 ns         89.2 ns      7798969
BM_linearSearch_visibleInit< Animal >/512                                                684 ns          684 ns      1010620
BM_linearSearch_visibleInit< Animal >/4096                                             10183 ns        10181 ns        68753
BM_linearSearch_visibleInit< Animal >/32768                                           208919 ns       208509 ns         3349
BM_linearSearch_visibleInit< Animal >/262144                                         3986260 ns      3985541 ns          174
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/1                               5.15 ns         5.15 ns    133703008
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/8                               16.6 ns         16.5 ns     42509335
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/64                               113 ns          113 ns      6193792
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/512                              836 ns          836 ns       830356
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/4096                           10656 ns        10632 ns        65578
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/32768                          97575 ns        97551 ns         6968
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/262144                       2173203 ns      2172760 ns          319
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/1             3.95 ns         3.95 ns    176889618
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/8             14.6 ns         14.6 ns     48893922
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/64            95.7 ns         95.7 ns      7299376
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/512            732 ns          732 ns       942114
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/4096          7896 ns         7893 ns        87836
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/32768        75723 ns        75701 ns         9196
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/262144     1535836 ns      1535364 ns          450
BM_linearSearch_hiddenInit< Animal >/1                                                  3.87 ns         3.87 ns    180522178
BM_linearSearch_hiddenInit< Animal >/8                                                  13.4 ns         13.4 ns     52259409
BM_linearSearch_hiddenInit< Animal >/64                                                 89.5 ns         89.5 ns      7795769
BM_linearSearch_hiddenInit< Animal >/512                                                 689 ns          689 ns       993027
BM_linearSearch_hiddenInit< Animal >/4096                                              10365 ns        10363 ns        67535
BM_linearSearch_hiddenInit< Animal >/32768                                            206674 ns       206641 ns         3376
BM_linearSearch_hiddenInit< Animal >/262144                                          4038407 ns      4037034 ns          173
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/1                                4.88 ns         4.88 ns    142995270
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/8                                16.5 ns         16.5 ns     42498518
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/64                                111 ns          111 ns      6269956
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/512                               840 ns          840 ns       838201
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/4096                            10952 ns        10953 ns        63588
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/32768                           99285 ns        99285 ns         6621
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/262144                        2297590 ns      2297474 ns          305
BM_linearSearch_devirtualized/1                                                         1.54 ns         1.54 ns    453493929
BM_linearSearch_devirtualized/8                                                         5.91 ns         5.91 ns    117701605
BM_linearSearch_devirtualized/64                                                        29.3 ns         29.3 ns     23822477
BM_linearSearch_devirtualized/512                                                        464 ns          464 ns      1504664
BM_linearSearch_devirtualized/4096                                                      4654 ns         4653 ns       150281
BM_linearSearch_devirtualized/32768                                                    80553 ns        80551 ns         8581
BM_linearSearch_devirtualized/262144                                                 1213397 ns      1213332 ns          580
```
</details>

Preliminary investigations seem to indicate that clang is better at performing devirtualization of member functions when using `polymorphic_variant`,
leading to better performance. Therefore, if possible, it is recommended to compile your project using this library by clang instead of gcc (or MSVC
for that matter).

