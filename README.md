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
	GIT_TAG v2.0.0
)
FetchContent_MakeAvailable(polymorphic_variant)
```
In either case, the library will provide the `polymorphic_variant::polymorphic_variant` target that you can simply link your target(s) to in order for
the include-paths and compile flags to be set accordingly.


## Building

The library itself does not require building as it is header-only. However, if you want to run the test cases, the procedure is as follows:
```bash
mkdir build
cd build
cmake ..
cmake --build .

# Run the tests
ctest --output-on-failure
```

There are two noteworthy options that define how `polymorphic_variant` will be built:
- `PV_USE_VISIT_ACCESS` - this controls whether to use `std::visit` when accessing the underlying `std::variant` instead of using pointers. The latter
  is more efficient from a theoretic point of view but apparently some compilers have gotten really good at optimizing `std::visit` specifically that
  its use is actually beneficial for things like devirtualization. You can run the benchmarks to see what works best for your case. By default, this
  version is `OFF`.
- `PV_EXPLOIT_SHARED_STORAGE` - if using pointer-based access mechanics internally, this controls whether the pointer offsets from the internal
  variant's address to the address of the currently active element are re-computed every time or assumed to be the same for all elements.
  Standard-compliant variant implementations should never require a per-element addressing. This option is enabled by default if a test program
  thinks that this is safe to do (as should be the case).


## Performance

Based on the [benchmarks](./benchmarks) I have performed on my system (Linux, GCC 14.2.0) it seems that accessing a base-class function via
`polymorphic_variant` is about as fast as accessing it via a `std::unique_ptr`. However, in cases in which the compiler can devirtualize the call,
because it knows the actual type of the accessed object, accessing via `unique_ptr` is about twice as fast as via `polymorphic_variant` because GCC
(atm) won't devirtualize when accessing the object via `polymorphic_variant`.

Clang seems to be significantly better at devirtualizing function calls through a `polymorphic_variant`.

In a test case of searching a `std::vector` of objects for an element that yields a particular value by calling a base-class method, using
`unique_ptr` yields comparable performance as `polymorphic_variant`. However, for large vectors using `polymorphic_variant` becomes significantly
faster. This is likely due to beneficial cache effects as `polymorphic_variant` stores its values inside instead of holding a pointer to a remote
memory address (that needs to be looked up and which generally can't be prefetched).

Interestingly, modern compilers seem to have become very good at optimizing regular `std::variant` accesses. This makes `std::variant` more performant
than using `polymorphic_variant`. It is possible that setting `PV_USE_VISIT_ACCESS=ON` when building this library can yield improved performance for
`polymorphic_variant` but the effect depends on the use-case (for other use-cases performance might degrade) and `std::variant` might still end up
being faster. So in the end, it seems that there is a performance price to pay for the convenience `polymorphic_variant` provides over `std::variant`
depending on how specific your compiler's optimizations are to uses of `std::variant` directly.

If you want to build and check the benchmarks yourself, use `-DPV_BUILD_BENCHMARKS=ON` when invoking cmake.

<details>
	<summary>GCC 14.2.0 Benchmark results</summary>

```
2025-08-30T14:47:21+02:00
Running ./benchmarks/polymorphic_variant_benchmark
Run on (8 X 3900 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 0.23, 0.40, 0.70
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                  Time             CPU   Iterations
----------------------------------------------------------------------------------------------------------------------------
BM_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >                           1.84 ns         1.83 ns    384343481
BM_visibleInit< Animal >                                                                1.03 ns         1.03 ns    674358912
BM_visibleInit< std::variant< Dog, Cat > >                                              1.04 ns         1.04 ns    671576544
BM_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >                            2.32 ns         2.32 ns    301752011
BM_hiddenInit< Animal >                                                                 1.81 ns         1.81 ns    385131510
BM_hiddenInit< std::variant< Dog, Cat > >                                               1.03 ns         1.03 ns    673643302
BM_devirtualized                                                                        1.04 ns         1.04 ns    669941564
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/1            4.13 ns         4.13 ns    169546284
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/8            13.7 ns         13.7 ns     51154276
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/64           93.0 ns         93.0 ns      7463540
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/512           815 ns          815 ns       855156
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/4096        11892 ns        11890 ns        58860
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/32768      113085 ns       113076 ns         5860
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/262144    3099293 ns      3098837 ns          227
BM_linearSearch_visibleInit< Animal >/1                                                 4.17 ns         4.17 ns    167103591
BM_linearSearch_visibleInit< Animal >/8                                                 13.2 ns         13.2 ns     52981647
BM_linearSearch_visibleInit< Animal >/64                                                89.1 ns         89.1 ns      7766628
BM_linearSearch_visibleInit< Animal >/512                                                684 ns          684 ns      1023369
BM_linearSearch_visibleInit< Animal >/4096                                             10230 ns        10230 ns        68151
BM_linearSearch_visibleInit< Animal >/32768                                           205184 ns       205176 ns         3375
BM_linearSearch_visibleInit< Animal >/262144                                         4005787 ns      4005533 ns          173
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/1                               4.38 ns         4.38 ns    159562957
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/8                               11.8 ns         11.8 ns     59045207
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/64                              71.2 ns         71.2 ns      9618706
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/512                              607 ns          607 ns      1138300
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/4096                            9453 ns         9451 ns        73450
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/32768                          91802 ns        91787 ns         7280
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/262144                       2243734 ns      2243450 ns          315
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/1             4.63 ns         4.63 ns    151202772
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/8             14.7 ns         14.7 ns     47553090
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/64            99.3 ns         99.3 ns      7019800
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/512            798 ns          798 ns       867042
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/4096         11696 ns        11690 ns        59557
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/32768       108777 ns       108737 ns         6052
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/262144     2938770 ns      2938199 ns          238
BM_linearSearch_hiddenInit< Animal >/1                                                  4.13 ns         4.13 ns    170030417
BM_linearSearch_hiddenInit< Animal >/8                                                  13.2 ns         13.2 ns     53195457
BM_linearSearch_hiddenInit< Animal >/64                                                 88.8 ns         88.8 ns      7828640
BM_linearSearch_hiddenInit< Animal >/512                                                 676 ns          676 ns      1018613
BM_linearSearch_hiddenInit< Animal >/4096                                              10057 ns        10057 ns        69017
BM_linearSearch_hiddenInit< Animal >/32768                                            208723 ns       208719 ns         3327
BM_linearSearch_hiddenInit< Animal >/262144                                          4056258 ns      4056069 ns          172
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/1                                4.12 ns         4.12 ns    169585500
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/8                                10.5 ns         10.5 ns     66286025
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/64                               53.6 ns         53.6 ns     12787419
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/512                               662 ns          662 ns      1033538
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/4096                             9495 ns         9494 ns        73331
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/32768                           92304 ns        92295 ns         7219
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/262144                        2223290 ns      2223013 ns          312
BM_linearSearch_devirtualized/1                                                         1.55 ns         1.55 ns    451138413
BM_linearSearch_devirtualized/8                                                         5.40 ns         5.40 ns    125267607
BM_linearSearch_devirtualized/64                                                        28.2 ns         28.1 ns     24831312
BM_linearSearch_devirtualized/512                                                        590 ns          590 ns      1162867
BM_linearSearch_devirtualized/4096                                                      9162 ns         9158 ns        76068
BM_linearSearch_devirtualized/32768                                                    54519 ns        54512 ns        12674
BM_linearSearch_devirtualized/262144                                                 1299856 ns      1299703 ns          538
```
</details>

<details>
	<summary>Clang 19.1.7 Benchmark results</summary>

```
2025-08-30T14:54:25+02:00
Running ./benchmarks/polymorphic_variant_benchmark
Run on (8 X 3900 MHz CPU s)
CPU Caches:
  L1 Data 32 KiB (x4)
  L1 Instruction 32 KiB (x4)
  L2 Unified 256 KiB (x4)
  L3 Unified 8192 KiB (x1)
Load Average: 1.27, 0.88, 0.79
***WARNING*** CPU scaling is enabled, the benchmark real time measurements may be noisy and will incur extra overhead.
----------------------------------------------------------------------------------------------------------------------------
Benchmark                                                                                  Time             CPU   Iterations
----------------------------------------------------------------------------------------------------------------------------
BM_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >                           1.59 ns         1.59 ns    402214651
BM_visibleInit< Animal >                                                                1.55 ns         1.55 ns    452559140
BM_visibleInit< std::variant< Dog, Cat > >                                              1.56 ns         1.56 ns    451832966
BM_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >                            1.55 ns         1.55 ns    449718171
BM_hiddenInit< Animal >                                                                 1.55 ns         1.55 ns    451772192
BM_hiddenInit< std::variant< Dog, Cat > >                                               1.55 ns         1.55 ns    452602909
BM_devirtualized                                                                        1.56 ns         1.56 ns    449792581
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/1            3.88 ns         3.88 ns    180111826
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/8            14.4 ns         14.4 ns     48259991
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/64            101 ns          101 ns      6868848
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/512           801 ns          801 ns       863201
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/4096        11666 ns        11665 ns        59595
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/32768      109112 ns       109103 ns         6118
BM_linearSearch_visibleInit< pv::polymorphic_variant< Animal, Dog, Cat > >/262144    3059592 ns      3059063 ns          241
BM_linearSearch_visibleInit< Animal >/1                                                 2.86 ns         2.86 ns    243045105
BM_linearSearch_visibleInit< Animal >/8                                                 12.6 ns         12.6 ns     55661548
BM_linearSearch_visibleInit< Animal >/64                                                88.4 ns         88.4 ns      7852825
BM_linearSearch_visibleInit< Animal >/512                                                688 ns          688 ns       999214
BM_linearSearch_visibleInit< Animal >/4096                                             10168 ns        10168 ns        68536
BM_linearSearch_visibleInit< Animal >/32768                                           212332 ns       212320 ns         3243
BM_linearSearch_visibleInit< Animal >/262144                                         4006024 ns      4005703 ns          175
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/1                               4.26 ns         4.26 ns    164144849
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/8                               14.2 ns         14.2 ns     49198106
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/64                               102 ns          102 ns      6838952
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/512                              803 ns          803 ns       860913
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/4096                            9439 ns         9438 ns        73973
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/32768                          91926 ns        91915 ns         7264
BM_linearSearch_visibleInit< std::variant< Dog, Cat > >/262144                       2129923 ns      2129734 ns          327
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/1             3.61 ns         3.61 ns    193801199
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/8             14.2 ns         14.2 ns     49224458
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/64            97.8 ns         97.7 ns      7094199
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/512            757 ns          757 ns       914032
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/4096         11541 ns        11541 ns        60302
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/32768       108369 ns       108353 ns         6122
BM_linearSearch_hiddenInit< pv::polymorphic_variant< Animal, Dog, Cat > >/262144     2810828 ns      2810641 ns          249
BM_linearSearch_hiddenInit< Animal >/1                                                  3.10 ns         3.10 ns    226005677
BM_linearSearch_hiddenInit< Animal >/8                                                  12.8 ns         12.8 ns     54355800
BM_linearSearch_hiddenInit< Animal >/64                                                 88.7 ns         88.7 ns      7828191
BM_linearSearch_hiddenInit< Animal >/512                                                 703 ns          703 ns      1003604
BM_linearSearch_hiddenInit< Animal >/4096                                              10225 ns        10221 ns        67596
BM_linearSearch_hiddenInit< Animal >/32768                                            212103 ns       212066 ns         3310
BM_linearSearch_hiddenInit< Animal >/262144                                          4002696 ns      4002287 ns          175
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/1                                4.26 ns         4.26 ns    164730801
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/8                                14.2 ns         14.2 ns     49126155
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/64                                102 ns          102 ns      6824549
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/512                               776 ns          776 ns       888334
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/4096                             9392 ns         9391 ns        73983
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/32768                           91646 ns        91635 ns         7242
BM_linearSearch_hiddenInit< std::variant< Dog, Cat > >/262144                        2132448 ns      2132242 ns          324
BM_linearSearch_devirtualized/1                                                         2.58 ns         2.58 ns    270374282
BM_linearSearch_devirtualized/8                                                         12.3 ns         12.3 ns     56225872
BM_linearSearch_devirtualized/64                                                        89.1 ns         89.1 ns      7658959
BM_linearSearch_devirtualized/512                                                        669 ns          668 ns      1028790
BM_linearSearch_devirtualized/4096                                                      9547 ns         9545 ns        72477
BM_linearSearch_devirtualized/32768                                                    67420 ns        67401 ns        10292
BM_linearSearch_devirtualized/262144                                                 1524086 ns      1523853 ns          463
```
</details>

