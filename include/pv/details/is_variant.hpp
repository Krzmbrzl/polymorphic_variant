// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_DETAILS_IS_VARIANT_HPP__
#define PV_DETAILS_IS_VARIANT_HPP__

#include <type_traits>
#include <variant>

namespace pv::details {

template< typename T > struct is_variant : public std::false_type {};

template< typename... Types > struct is_variant< std::variant< Types... > > : public std::true_type {};

template< typename T > constexpr bool is_variant_v = is_variant< T >::value;

} // namespace pv::details

#endif // PV_DETAILS_IS_VARIANT_HPP__
