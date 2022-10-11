// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_FREEFUNCTIONS_HPP_
#define PV_FREEFUNCTIONS_HPP_

#include "pv/details/has_operator.hpp"
#include "pv/details/variant_converter.hpp"
#include "pv/polymorphic_variant.hpp"

#include <type_traits>
#include <variant>

namespace pv {

namespace {
	template< typename > constexpr bool is_polymorphic_variant_v = false;
	template< typename Base, typename... Types >
	constexpr bool is_polymorphic_variant_v< polymorphic_variant< Base, Types... > > = true;

	template< typename T >
	using enable_if_polymorphic_variant_t = std::enable_if_t< is_polymorphic_variant_v< T >, void >;
} // namespace

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


///////////////////////////////////////////////////////////////
///////////////// BINARY OPERATORS ////////////////////////////
///////////////////////////////////////////////////////////////

#define BINARY_OPS             \
	PROCESS_OP(==, equals)     \
	PROCESS_OP(!=, unequals)   \
	PROCESS_OP(<, less)        \
	PROCESS_OP(>, greater)     \
	PROCESS_OP(<=, less_eq)    \
	PROCESS_OP(>=, greater_eq) \
	PROCESS_OP(+, add)         \
	PROCESS_OP(-, subtract)    \
	PROCESS_OP(/, divide)      \
	PROCESS_OP(*, multiply)

#define PROCESS_OP(the_op, name)                                                                                  \
	template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant > >                           \
	bool operator the_op(const Variant &lhs, const Variant &rhs) {                                                \
		static_assert(details::has_##name##_v< typename Variant::base_type >,                                     \
					  "The base type interface does not provide the requested operator");                         \
		return lhs.get() the_op rhs.get();                                                                        \
	}                                                                                                             \
	template< typename Variant, typename Base, typename = enable_if_polymorphic_variant_t< Variant >,             \
			  typename =                                                                                          \
				  std::enable_if_t< std::is_same_v< typename Variant::base_type, std::decay_t< Base > >, void > > \
	bool operator the_op(const Variant &lhs, const Base &rhs) {                                                   \
		static_assert(details::has_##name##_v< typename Variant::base_type >,                                     \
					  "The base type interface does not provide the requested operator");                         \
		return lhs.get() the_op rhs;                                                                              \
	}                                                                                                             \
	template< typename Variant, typename Base, typename = enable_if_polymorphic_variant_t< Variant >,             \
			  typename = std::enable_if_t< std::is_same_v< typename Variant::base_type, Base >, void > >          \
	bool operator the_op(const Base &lhs, const Variant &rhs) {                                                   \
		static_assert(details::has_##name##_v< typename Variant::base_type >,                                     \
					  "The base type interface does not provide the requested operator");                         \
		return lhs the_op rhs.get();                                                                              \
	}

BINARY_OPS

#undef PROCESS_OP
#undef BINARY_OPS

} // namespace pv

#endif // PV_FREEFUNCTIONS_HPP_
