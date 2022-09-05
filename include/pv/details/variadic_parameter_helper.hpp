// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

namespace pv {
namespace details {

	template< typename T1, typename... Rest > struct first_variadic_parameter { using type = T1; };
	template< typename T1 > struct first_variadic_parameter< T1 > { using type = T1; };

	template< typename T1, typename... Rest > struct last_variadic_parameter {
		using type = typename last_variadic_parameter< Rest... >::value;
	};
	template< typename T1 > struct last_variadic_parameter< T1 > { using type = T1; };

} // namespace details
} // namespace pv
