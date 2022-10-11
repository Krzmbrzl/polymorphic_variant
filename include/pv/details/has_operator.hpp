// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_DETAILS_HAS_OPERATOR_HPP__
#define PV_DETAILS_HAS_OPERATOR_HPP__

#include <type_traits>

namespace pv::details {

#define ALL_BINARY_OPERATORS                 \
	PROCESS_OPERATOR(==, equals)             \
	PROCESS_OPERATOR(!=, unequals)           \
	PROCESS_OPERATOR(<, less)                \
	PROCESS_OPERATOR(>, greater)             \
	PROCESS_OPERATOR(<=, less_eq)            \
	PROCESS_OPERATOR(>=, greater_eq)         \
	PROCESS_OPERATOR(+, add)                 \
	PROCESS_OPERATOR(-, subtract)            \
	PROCESS_OPERATOR(*, multiply)            \
	PROCESS_OPERATOR(/, divide)              \
	PROCESS_OPERATOR(%, modulo)              \
	PROCESS_OPERATOR(&&, and)                \
	PROCESS_OPERATOR(||, or)                 \
	PROCESS_OPERATOR(&, bitwise_and)         \
	PROCESS_OPERATOR(|, bitwise_or)          \
	PROCESS_OPERATOR(^, bitwise_xor)         \
	PROCESS_OPERATOR(<<, left_shift)         \
	PROCESS_OPERATOR(>>, right_shift)        \
	PROCESS_OPERATOR(+=, add_assign)         \
	PROCESS_OPERATOR(-=, subtract_assign)    \
	PROCESS_OPERATOR(*=, multiply_assign)    \
	PROCESS_OPERATOR(/=, divide_assign)      \
	PROCESS_OPERATOR(%=, modulo_assign)      \
	PROCESS_OPERATOR(&=, bitwise_and_assign) \
	PROCESS_OPERATOR(|=, bitwise_or_assign)  \
	PROCESS_OPERATOR(^=, bitwise_xor_assign) \
	PROCESS_OPERATOR(<<=, left_shift_assign) \
	PROCESS_OPERATOR(>>=, right_shift_assign)

#define ALL_UNARY_OPERATORS          \
	PROCESS_OPERATOR(+, unary_plus)  \
	PROCESS_OPERATOR(-, unary_minus) \
	PROCESS_OPERATOR(!, negation)    \
	PROCESS_OPERATOR(~, bitwise_negation)

#define ALL_INCREMENT_OPERATORS             \
	PROCESS_OPERATOR(++, prefix_increment)  \
	PROCESS_OPERATOR(--, prefix_decrement)  \
	PROCESS_OPERATOR(++, postfix_increment) \
	PROCESS_OPERATOR(--, postfix_decrement)

#define ALL_SPECIAL_OPERATORS PROCESS_OPERATOR([], subscript)

/**
 * Define a type_trait that can be used to check whether using a given operator with the given types
 * results in a valid expression.
 */
#define DEFINE_CAN_BINARY_OP(op, name)     \
	template< typename LHS, typename RHS > \
	using can_##name##_t = decltype(std::declval< LHS >() op std::declval< RHS >())
#define DEFINE_CAN_UNARY_OP(op, name) template< typename T > using can_##name##_t = decltype(op std::declval< T >())
/**
 * Define type_traits to check whether there exists a binary operator acting on the two given types
 */
#define DEFINE_HAS_BINARY_OP(name)                                                                               \
	template< typename LHS, typename RHS = LHS, typename = void > struct has_##name : public std::false_type {}; \
	template< typename LHS, typename RHS >                                                                       \
	struct has_##name< LHS, RHS, std::void_t< can_##name##_t< LHS, RHS > > > : public std::true_type {};         \
	template< typename LHS >                                                                                     \
	struct has_##name< LHS, LHS, std::void_t< can_##name##_t< LHS, LHS > > > : public std::true_type {};         \
	template< typename LHS, typename RHS = LHS > constexpr bool has_##name##_v = has_##name< LHS, RHS >::value;
#define DEFINE_HAS_UNARY_OP(name)                                                                                 \
	template< typename T, typename = void > struct has_##name : public std::false_type {};                        \
	template< typename T > struct has_##name< T, std::void_t< can_##name##_t< T > > > : public std::true_type {}; \
	template< typename T > constexpr bool has_##name##_v = has_##name< T >::value;

//////////////////
// Special cases
//////////////////

// subscript
template< typename T, typename Index > using can_subscript_t = decltype(std::declval< T >()[std::declval< Index >()]);
template< typename T, typename Index, typename = void > struct has_subscript : public std::false_type {};
template< typename T, typename Index >
struct has_subscript< T, Index, std::void_t< can_subscript_t< T, Index > > > : public std::true_type {};
template< typename T, typename Index > constexpr bool has_subscript_v = has_subscript< T, Index >::value;
// prefix_increment
template< typename T > using can_prefix_increment_t = decltype(++std::declval< T >());
// postfix_increment
template< typename T > using can_postfix_increment_t = decltype(std::declval< T >()++);
// prefix_decrement
template< typename T > using can_prefix_decrement_t = decltype(--std::declval< T >());
// postfix_decrement
template< typename T > using can_postfix_decrement_t = decltype(std::declval< T >()--);


#define PROCESS_OPERATOR(the_operator, name)  \
	DEFINE_CAN_BINARY_OP(the_operator, name); \
	DEFINE_HAS_BINARY_OP(name);

ALL_BINARY_OPERATORS

#undef PROCESS_OPERATOR

#define PROCESS_OPERATOR(the_operator, name) \
	DEFINE_CAN_UNARY_OP(the_operator, name); \
	DEFINE_HAS_UNARY_OP(name);

ALL_UNARY_OPERATORS

#undef PROCESS_OPERATOR

#define PROCESS_OPERATOR(the_operator, name) DEFINE_HAS_UNARY_OP(name);

ALL_INCREMENT_OPERATORS

#undef PROCESS_OPERATOR
#undef DEFINE_OP_BINARY_TYPE
#undef DEFINE_HAS_BINARY_OP
#undef DEFINE_CAN_BINARY_OP
#undef DEFINE_CAN_UNNARY_OP

/**
 * Define enable_if-shortcuts conditioned on the existence of the different binary operators for the provided types
 */
#define DEFINE_ENABLE_IF_HAS_BINARY_OP(name)     \
	template< typename LHS, typename RHS = LHS > \
	using enable_if_has_##name##_t = std::enable_if_t< has_##name##_v< LHS, RHS >, void >;

#define PROCESS_OPERATOR(the_operator, name) DEFINE_ENABLE_IF_HAS_BINARY_OP(name);

ALL_BINARY_OPERATORS
ALL_UNARY_OPERATORS
ALL_INCREMENT_OPERATORS
ALL_SPECIAL_OPERATORS

#undef PROCESS_OPERATOR
#undef DEFINE_ENABLE_IF_HAS_BINARY_OP
#undef ALL_BINARY_OPERATORS
#undef ALL_UNARY_OPERATORS
#undef ALL_INCREMENT_OPERATORS
#undef ALL_SPECIAL_OPERATORS

} // namespace pv::details

#endif // PV_DETAILS_HAS_OPERATOR_HPP__
