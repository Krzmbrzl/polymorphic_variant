# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

add_executable(operators_test "operators_test.cpp")

target_link_libraries(operators_test PUBLIC polymorphic_variant polymorphic_variant_compiler_flags)

register_test(TARGETS operators_test)
