# Use of this source code is governed by a BSD-style license that can
# be found in the LICENSE file at the root of the source tree or at
# <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

add_executable(main_test "main_test.cpp")

target_link_libraries(main_test PUBLIC polymorphic_variant)

register_test(TARGETS main_test)
