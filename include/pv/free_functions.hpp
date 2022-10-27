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
	template< typename T >
	using enable_if_not_polymorphic_variant_t = std::enable_if_t< !is_polymorphic_variant_v< T >, void >;
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
//////////////////////// OPERATORS ////////////////////////////
///////////////////////////////////////////////////////////////

#define BINARY_OPS                    \
	PROCESS_OPERATOR(==, equals)      \
	PROCESS_OPERATOR(!=, unequals)    \
	PROCESS_OPERATOR(<, less)         \
	PROCESS_OPERATOR(>, greater)      \
	PROCESS_OPERATOR(<=, less_eq)     \
	PROCESS_OPERATOR(>=, greater_eq)  \
	PROCESS_OPERATOR(+, add)          \
	PROCESS_OPERATOR(-, subtract)     \
	PROCESS_OPERATOR(*, multiply)     \
	PROCESS_OPERATOR(/, divide)       \
	PROCESS_OPERATOR(%, modulo)       \
	PROCESS_OPERATOR(&&, and)         \
	PROCESS_OPERATOR(||, or)          \
	PROCESS_OPERATOR(&, bitwise_and)  \
	PROCESS_OPERATOR(|, bitwise_or)   \
	PROCESS_OPERATOR(^, bitwise_xor)  \
	PROCESS_OPERATOR(<<, left_shift)  \
	PROCESS_OPERATOR(>>, right_shift) \
	PROCESS_OPERATOR(+=, add_assign)

#define BINARY_MUTATING_OPS                  \
	PROCESS_OPERATOR(-=, subtract_assign)    \
	PROCESS_OPERATOR(*=, multiply_assign)    \
	PROCESS_OPERATOR(/=, divide_assign)      \
	PROCESS_OPERATOR(%=, modulo_assign)      \
	PROCESS_OPERATOR(&=, bitwise_and_assign) \
	PROCESS_OPERATOR(|=, bitwise_or_assign)  \
	PROCESS_OPERATOR(^=, bitwise_xor_assign) \
	PROCESS_OPERATOR(<<=, left_shift_assign) \
	PROCESS_OPERATOR(>>=, right_shift_assign)

#define UNARY_OPS                    \
	PROCESS_OPERATOR(+, unary_plus)  \
	PROCESS_OPERATOR(-, unary_minus) \
	PROCESS_OPERATOR(!, negation)    \
	PROCESS_OPERATOR(~, bitwise_negation)

// Binary operators

#define PROCESS_OPERATOR(the_op, name)                                                                         \
	template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,                         \
			  typename = details::enable_if_has_##name##_t< const typename Variant::base_type & > >            \
	decltype(auto) operator the_op(const Variant &lhs, const Variant &rhs) {                                   \
		return lhs.get() the_op rhs.get();                                                                     \
	}                                                                                                          \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,             \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                             \
			  typename = details::enable_if_has_##name##_t< const typename Variant::base_type &, const T & > > \
	decltype(auto) operator the_op(const Variant &lhs, const T &rhs) {                                         \
		return lhs.get() the_op rhs;                                                                           \
	}                                                                                                          \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,             \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                             \
			  typename = details::enable_if_has_##name##_t< const T &, const typename Variant::base_type & > > \
	decltype(auto) operator the_op(const T &lhs, const Variant &rhs) {                                         \
		return lhs the_op rhs.get();                                                                           \
	}

BINARY_OPS

#undef PROCESS_OPERATOR

// Binary mutating operators

#define PROCESS_OPERATOR(the_op, name)                                                                   \
	template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,                   \
			  typename = details::enable_if_has_##name##_t< typename Variant::base_type &,               \
															const typename Variant::base_type & > >      \
	decltype(auto) operator the_op(Variant &lhs, const Variant &rhs) {                                   \
		return lhs.get() the_op rhs.get();                                                               \
	}                                                                                                    \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,       \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                       \
			  typename = details::enable_if_has_##name##_t< typename Variant::base_type &, const T & > > \
	decltype(auto) operator the_op(Variant &lhs, const T &rhs) {                                         \
		return lhs.get() the_op rhs;                                                                     \
	}                                                                                                    \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,       \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                       \
			  typename = details::enable_if_has_##name##_t< T &, const typename Variant::base_type & > > \
	decltype(auto) operator the_op(T &lhs, const Variant &rhs) {                                         \
		return lhs the_op rhs.get();                                                                     \
	}

BINARY_MUTATING_OPS

#undef PROCESS_OPERATOR


// Unary operators

#define PROCESS_OPERATOR(the_op, name)                                                              \
	template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,              \
			  typename = details::enable_if_has_##name##_t< const typename Variant::base_type & > > \
	decltype(auto) operator the_op(const Variant &variant) {                                        \
		return the_op variant.get();                                                                \
	}

UNARY_OPS

#undef PROCESS_OPERATOR
#undef BINARY_OPS
#undef BINARY_MUTATING_OPS
#undef UNARY_OPS

// Special operators

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = details::enable_if_has_prefix_increment_t< typename Variant::base_type & > >
decltype(auto) operator++(Variant &variant) {
	return ++variant.get();
}

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = details::enable_if_has_prefix_decrement_t< typename Variant::base_type & > >
decltype(auto) operator--(Variant &variant) {
	return --variant.get();
}

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = details::enable_if_has_postfix_increment_t< typename Variant::base_type & > >
decltype(auto) operator++(Variant &variant, int) {
	return variant.get()++;
}

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = details::enable_if_has_postfix_decrement_t< typename Variant::base_type & > >
decltype(auto) operator--(Variant &variant, int) {
	return variant.get()--;
}

// subscript operator is implemented as member function (as is required)

} // namespace pv

#endif // PV_FREEFUNCTIONS_HPP_
