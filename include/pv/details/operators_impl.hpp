// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_OPERATORSIMPL_HPP_
#define PV_OPERATORSIMPL_HPP_

#include "pv/details/has_operator.hpp"
#include "pv/details/polymorphic_variant_impl.hpp"

#include <type_traits>

namespace pv::details {

namespace {
	template< typename > constexpr bool is_polymorphic_variant_v = false;
	template< typename Base, typename... Types >
	constexpr bool is_polymorphic_variant_v< polymorphic_variant< Base, Types... > > = true;

	template< typename T >
	using enable_if_polymorphic_variant_t = std::enable_if_t< is_polymorphic_variant_v< T >, void >;
	template< typename T >
	using enable_if_not_polymorphic_variant_t = std::enable_if_t< !is_polymorphic_variant_v< T >, void >;
} // namespace

///////////////////////////////////////////////////////////////
//////////////////////// OPERATORS ////////////////////////////
///////////////////////////////////////////////////////////////

#define PV_BINARY_OPS                    \
	PV_PROCESS_OPERATOR(==, equals)      \
	PV_PROCESS_OPERATOR(!=, unequals)    \
	PV_PROCESS_OPERATOR(<, less)         \
	PV_PROCESS_OPERATOR(>, greater)      \
	PV_PROCESS_OPERATOR(<=, less_eq)     \
	PV_PROCESS_OPERATOR(>=, greater_eq)  \
	PV_PROCESS_OPERATOR(+, add)          \
	PV_PROCESS_OPERATOR(-, subtract)     \
	PV_PROCESS_OPERATOR(*, multiply)     \
	PV_PROCESS_OPERATOR(/, divide)       \
	PV_PROCESS_OPERATOR(%, modulo)       \
	PV_PROCESS_OPERATOR(&&, and)         \
	PV_PROCESS_OPERATOR(||, or)          \
	PV_PROCESS_OPERATOR(&, bitwise_and)  \
	PV_PROCESS_OPERATOR(|, bitwise_or)   \
	PV_PROCESS_OPERATOR(^, bitwise_xor)  \
	PV_PROCESS_OPERATOR(<<, left_shift)  \
	PV_PROCESS_OPERATOR(>>, right_shift) \
	PV_PROCESS_OPERATOR(+=, add_assign)

#define PV_BINARY_MUTATING_OPS                  \
	PV_PROCESS_OPERATOR(-=, subtract_assign)    \
	PV_PROCESS_OPERATOR(*=, multiply_assign)    \
	PV_PROCESS_OPERATOR(/=, divide_assign)      \
	PV_PROCESS_OPERATOR(%=, modulo_assign)      \
	PV_PROCESS_OPERATOR(&=, bitwise_and_assign) \
	PV_PROCESS_OPERATOR(|=, bitwise_or_assign)  \
	PV_PROCESS_OPERATOR(^=, bitwise_xor_assign) \
	PV_PROCESS_OPERATOR(<<=, left_shift_assign) \
	PV_PROCESS_OPERATOR(>>=, right_shift_assign)

#define PV_UNARY_OPS                    \
	PV_PROCESS_OPERATOR(+, unary_plus)  \
	PV_PROCESS_OPERATOR(-, unary_minus) \
	PV_PROCESS_OPERATOR(!, negation)    \
	PV_PROCESS_OPERATOR(~, bitwise_negation)

// Binary operators

#define PV_PROCESS_OPERATOR(the_op, name)                                                             \
	template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,                \
			  typename = enable_if_has_##name##_t< const typename Variant::base_type & > >            \
	decltype(auto) operator the_op(const Variant &lhs, const Variant &rhs) {                          \
		return lhs.get() the_op rhs.get();                                                            \
	}                                                                                                 \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,    \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                    \
			  typename = enable_if_has_##name##_t< const typename Variant::base_type &, const T & > > \
	decltype(auto) operator the_op(const Variant &lhs, const T &rhs) {                                \
		return lhs.get() the_op rhs;                                                                  \
	}                                                                                                 \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,    \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                    \
			  typename = enable_if_has_##name##_t< const T &, const typename Variant::base_type & > > \
	decltype(auto) operator the_op(const T &lhs, const Variant &rhs) {                                \
		return lhs the_op rhs.get();                                                                  \
	}

PV_BINARY_OPS

#undef PV_PROCESS_OPERATOR

// Binary mutating operators

#define PV_PROCESS_OPERATOR(the_op, name)                                                                          \
	template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,                             \
			  typename =                                                                                           \
				  enable_if_has_##name##_t< typename Variant::base_type &, const typename Variant::base_type & > > \
	decltype(auto) operator the_op(Variant &lhs, const Variant &rhs) {                                             \
		return lhs.get() the_op rhs.get();                                                                         \
	}                                                                                                              \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,                 \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                                 \
			  typename = enable_if_has_##name##_t< typename Variant::base_type &, const T & > >                    \
	decltype(auto) operator the_op(Variant &lhs, const T &rhs) {                                                   \
		return lhs.get() the_op rhs;                                                                               \
	}                                                                                                              \
	template< typename Variant, typename T, typename = enable_if_polymorphic_variant_t< Variant >,                 \
			  typename = enable_if_not_polymorphic_variant_t< T >,                                                 \
			  typename = enable_if_has_##name##_t< T &, const typename Variant::base_type & > >                    \
	decltype(auto) operator the_op(T &lhs, const Variant &rhs) {                                                   \
		return lhs the_op rhs.get();                                                                               \
	}

PV_BINARY_MUTATING_OPS

#undef PV_PROCESS_OPERATOR


// Unary operators

#define PV_PROCESS_OPERATOR(the_op, name)                                                  \
	template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,     \
			  typename = enable_if_has_##name##_t< const typename Variant::base_type & > > \
	decltype(auto) operator the_op(const Variant &variant) {                               \
		return the_op variant.get();                                                       \
	}

PV_UNARY_OPS

#undef PV_PROCESS_OPERATOR
#undef PV_BINARY_OPS
#undef PV_BINARY_MUTATING_OPS
#undef PV_UNARY_OPS

// Special operators

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = enable_if_has_prefix_increment_t< typename Variant::base_type & > >
decltype(auto) operator++(Variant &variant) {
	return ++variant.get();
}

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = enable_if_has_prefix_decrement_t< typename Variant::base_type & > >
decltype(auto) operator--(Variant &variant) {
	return --variant.get();
}

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = enable_if_has_postfix_increment_t< typename Variant::base_type & > >
decltype(auto) operator++(Variant &variant, int) {
	return variant.get()++;
}

template< typename Variant, typename = enable_if_polymorphic_variant_t< Variant >,
		  typename = enable_if_has_postfix_decrement_t< typename Variant::base_type & > >
decltype(auto) operator--(Variant &variant, int) {
	return variant.get()--;
}

// subscript operator is implemented as member function (as is required)

} // namespace pv::details

#endif // PV_OPERATORSIMPL_HPP_
