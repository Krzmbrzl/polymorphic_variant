// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_DETAILS_HAS_OPERATOR_HPP__
#define PV_DETAILS_HAS_OPERATOR_HPP__

#include <type_traits>

namespace pv::details {

#define ALL_OPERATORS                \
	PROCESS_OPERATOR(==, equals)     \
	PROCESS_OPERATOR(!=, unequals)   \
	PROCESS_OPERATOR(<, less)        \
	PROCESS_OPERATOR(>, greater)     \
	PROCESS_OPERATOR(<=, less_eq)    \
	PROCESS_OPERATOR(>=, greater_eq) \
	PROCESS_OPERATOR(+, add)         \
	PROCESS_OPERATOR(-, subtract)    \
	PROCESS_OPERATOR(/, divide)      \
	PROCESS_OPERATOR(*, multiply)

namespace op {
#define PROCESS_OPERATOR(the_op, name) name,
	enum Operator { ALL_OPERATORS };
#undef PROCESS_OPERATOR
} // namespace op

#define DEFINE_CAN_OP(op, name)            \
	template< typename LHS, typename RHS > \
	using can_##name##_t = decltype(std::declval< LHS >() op std::declval< RHS >())
#define DEFINE_HAS_OP(name)                                                                                      \
	template< typename LHS, typename RHS = LHS, typename = void > struct has_##name : public std::false_type {}; \
	template< typename LHS, typename RHS >                                                                       \
	struct has_##name< LHS, RHS, std::void_t< can_##name##_t< LHS, RHS > > > : public std::true_type {};         \
	template< typename LHS >                                                                                     \
	struct has_##name< LHS, LHS, std::void_t< can_##name##_t< LHS, LHS > > > : public std::true_type {};         \
	template< typename LHS, typename RHS = LHS > constexpr bool has_##name##_v = has_##name< LHS, RHS >::value;

#define PROCESS_OPERATOR(the_operator, name) \
	DEFINE_CAN_OP(the_operator, name);       \
	DEFINE_HAS_OP(name);

ALL_OPERATORS

#undef PROCESS_OPERATOR
#undef DEFINE_OP_TYPE
#undef DEFINE_HAS_OP

#define DEFINE_ENABLE_IF_HAS_OP(name)            \
	template< typename LHS, typename RHS = LHS > \
	using enable_if_has_##name##_t = std::enable_if_t< has_##name##_v< LHS, RHS >, void >;

#define PROCESS_OPERATOR(the_operator, name) DEFINE_ENABLE_IF_HAS_OP(name);

ALL_OPERATORS

#undef PROCESS_OPERATOR
#undef DEFINE_ENABLE_IF_HAS_OP


template< int op, typename LHS, typename RHS, typename = void > struct has_operator : public std::false_type {};

#define SPECIALIZE(name)                   \
	template< typename LHS, typename RHS > \
	struct has_operator< op::name, LHS, RHS, enable_if_has_##name##_t< LHS, RHS > > : public std::true_type {};

#define PROCESS_OPERATOR(the_operator, name) SPECIALIZE(name);

ALL_OPERATORS

#undef PROCESS_OPERATOR
#undef SPECIALIZE

template< int op, typename LHS, typename RHS > constexpr bool has_operator_v = has_operator< op, LHS, RHS >::value;

#undef ALL_OPERATORS

} // namespace pv::details

#endif // PV_DETAILS_HAS_OPERATOR_HPP__
