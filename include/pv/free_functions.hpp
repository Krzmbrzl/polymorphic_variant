// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_FREEFUNCTIONS_HPP_
#define PV_FREEFUNCTIONS_HPP_

#include "pv/details/variant_converter.hpp"
#include "pv/polymorphic_variant.hpp"

#include <type_traits>
#include <variant>

namespace pv {

template< typename Visitor, typename... Variants > constexpr auto visit(Visitor &&vis, Variants &&... vars) {
	// std::visit can only deal with std::variants, so we have to make sure we pass polymorphic_variant as the
	// underlying variant object
	return std::visit(std::forward< Visitor >(vis), (details::variant_converter< Variants >::toVariant(vars), ...));
}

template< typename T, typename Variant > constexpr bool holds_alternative(const Variant &v) noexcept {
	return std::holds_alternative< T >(details::variant_converter< Variant >::toVariant(v));
}

template< std::size_t I, typename Variant >
constexpr std::variant_alternative_t< I, typename details::variant_converter< Variant >::variant_type > &
	get(Variant &v) {
	return std::get< I >(details::variant_converter< Variant >::toVariant(v));
}

template< std::size_t I, typename Variant >
constexpr std::variant_alternative_t< I, typename details::variant_converter< Variant >::variant_type > &&
	get(Variant &&v) {
	return std::get< I >(std::move(details::variant_converter< Variant >::toVariant(v)));
}

template< std::size_t I, typename Variant >
constexpr const std::variant_alternative_t< I, typename details::variant_converter< Variant >::variant_type > &
	get(const Variant &v) {
	return std::get< I >(details::variant_converter< Variant >::toVariant(v));
}

template< std::size_t I, typename Variant >
constexpr const std::variant_alternative_t< I, typename details::variant_converter< Variant >::variant_type > &&
	get(const Variant &&v) {
	return std::get< I >(std::move(details::variant_converter< Variant >::toVariant(v)));
}



template< typename T, typename Variant > constexpr T &get(Variant &v) {
	return std::get< T >(details::variant_converter< Variant >::toVariant(v));
}

template< typename T, typename Variant > constexpr const T &get(const Variant &v) {
	return std::get< T >(details::variant_converter< const Variant >::toVariant(v));
}


template< std::size_t I, typename Variant >
constexpr std::add_pointer_t<
	std::variant_alternative_t< I, typename details::variant_converter< Variant >::variant_type > >
	get_if(Variant *v) {
	return std::get_if< I >(details::variant_converter< Variant >::variant_ptr(v));
}

template< typename T, typename Variant > constexpr std::add_pointer_t< T > get_if(Variant *v) {
	return std::get_if< T >(details::variant_converter< Variant >::variant_ptr(v));
}


template< typename VariantA, typename VariantB > bool operator==(const VariantA &lhs, const VariantB &rhs) {
	return details::variant_converter< VariantA >::toVariant(lhs)
		   == details::variant_converter< VariantB >::toVariant(rhs);
}

template< typename VariantA, typename VariantB > bool operator!=(const VariantA &lhs, const VariantB &rhs) {
	return details::variant_converter< VariantA >::toVariant(lhs)
		   != details::variant_converter< VariantB >::toVariant(rhs);
}

template< typename VariantA, typename VariantB > bool operator>(const VariantA &lhs, const VariantB &rhs) {
	return details::variant_converter< VariantA >::toVariant(lhs)
		   > details::variant_converter< VariantB >::toVariant(rhs);
}

template< typename VariantA, typename VariantB > bool operator<(const VariantA &lhs, const VariantB &rhs) {
	return details::variant_converter< VariantA >::toVariant(lhs)
		   < details::variant_converter< VariantB >::toVariant(rhs);
}

template< typename VariantA, typename VariantB > bool operator>=(const VariantA &lhs, const VariantB &rhs) {
	return details::variant_converter< VariantA >::toVariant(lhs)
		   >= details::variant_converter< VariantB >::toVariant(rhs);
}

template< typename VariantA, typename VariantB > bool operator<=(const VariantA &lhs, const VariantB &rhs) {
	return details::variant_converter< VariantA >::toVariant(lhs)
		   <= details::variant_converter< VariantB >::toVariant(rhs);
}

} // namespace pv

#endif // PV_FREEFUNCTIONS_HPP_
