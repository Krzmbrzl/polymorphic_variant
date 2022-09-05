// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_POLYMORPHICVARIANT_HPP_
#define PV_POLYMORPHICVARIANT_HPP_

#include "details/variadic_parameter_helper.hpp"

#include <cassert>
#include <type_traits>
#include <variant>

namespace pv {

/**
 * This class is intended to be used for representing a polymorphic type that is part of a closed-set polymorphism (i.e.
 * all possible derived types are known at compile time). It uses a variant to store the respective object but supports
 * accessing the currently stored type by means of the base-class interface without the user having to worry about the
 * actual type that is currently being stored.
 *
 * Thus, effectively this class represents a way to obtain value-semantics for a set of polymorphic types.
 */
template< typename Base, typename... Types > class polymorphic_variant {
public:
	// Ensure that Types contains at least one type
	static_assert(sizeof...(Types) > 0, "Must provide at least one explicit sub-type");
	// Ensure template parameters are neither references nor pointers
	static_assert(!std::is_reference_v< Base >, "Base must not be given as a reference");
	static_assert(!std::is_pointer_v< Base >, "Base must not be given as a pointer");
	static_assert((!std::is_reference_v< Types > && ...), "None of the types may be given as a reference");
	static_assert((!std::is_pointer_v< Types > && ...), "None of the types may be given as a pointer");
	// Ensure that all types in Types actually inherit from Base
	static_assert((std::is_convertible_v< Types &, Base & > && ...), "All types must publicly inherit from Base");


	/**
	 * The type of the underlying variant object
	 */
	using variant_type = std::variant< Types... >;


	// Constructors (same as for std::variant)
	constexpr polymorphic_variant()
		: m_variant(),
		  m_base_ptr(&std::get< typename details::first_variadic_parameter< Types... >::type >(m_variant)) {}

	constexpr polymorphic_variant(const polymorphic_variant &other) = default;

	constexpr polymorphic_variant(polymorphic_variant &&other)
		: m_variant(std::move(other.m_variant)), m_base_ptr(other.m_base_ptr) {
		other.m_base_ptr = nullptr;
	}

	template< typename T >
	constexpr explicit polymorphic_variant(T &&t)
		: m_variant(std::forward< T >(t)), m_base_ptr(&std::get< std::decay_t< T > >(m_variant)) {}

	template< typename T, typename... Args >
	constexpr explicit polymorphic_variant(std::in_place_type_t< T >, Args &&... args)
		: m_variant(std::in_place_type< T >, args...), m_base_ptr(&std::get< std::decay_t< T > >(m_variant)) {}

	template< typename T, typename U, typename... Args >
	constexpr explicit polymorphic_variant(std::in_place_type_t< T >, std::initializer_list< U > il, Args &&... args)
		: m_variant(std::in_place_type< T >, std::forward(il), args...),
		  m_base_ptr(&std::get< std::decay_t< T > >(m_variant)) {}

	template< std::size_t I, typename... Args >
	constexpr explicit polymorphic_variant(std::in_place_index_t< I >, Args &&... args)
		: m_variant(std::in_place_index< I >, args...), m_base_ptr(&std::get< I >(m_variant)) {}

	template< std::size_t I, typename U, typename... Args >
	constexpr explicit polymorphic_variant(std::in_place_index_t< I >, std::initializer_list< U > il, Args &&... args)
		: m_variant(std::in_place_index< I >, std::forward(il), args...), m_base_ptr(&std::get< I >(m_variant)) {}

	~polymorphic_variant() = default;

	// Custom functions

	/**
	 * Gets the stored value as a base-class reference
	 */
	Base &get() {
		assert(m_base_ptr);
		assert(!m_variant.valueless_by_exception());
		return *m_base_ptr;
	}

	/**
	 * Gets the stored value as a base-class reference
	 */
	const Base &get() const {
		assert(m_base_ptr);
		assert(!m_variant.valueless_by_exception());
		return *m_base_ptr;
	}

	/**
	 * Accesses the currently stored object via the base-class interface
	 */
	Base *operator->() {
		assert(m_base_ptr);
		assert(!m_variant.valueless_by_exception());
		return m_base_ptr;
	}

	/**
	 * Accesses the currently stored object via the base-class interface
	 */
	const Base *operator->() const {
		assert(m_base_ptr);
		assert(!m_variant.valueless_by_exception());
		return m_base_ptr;
	}

	/**
	 * @returns The underlying variant object
	 */
	variant_type &variant() { return m_variant; }

	/**
	 * @returns The underlying variant object
	 */
	const variant_type &variant() const { return m_variant; }


	// Delegating functions for that part of the variant interface that also directly makes sense for
	// polymorphic_variant
	constexpr polymorphic_variant &operator=(const polymorphic_variant &rhs) = default;

	constexpr polymorphic_variant &operator=(polymorphic_variant &&rhs) = default;

	template< typename T > polymorphic_variant &operator=(T &&t) {
		m_variant = t;
		return *this;
	}


	constexpr std::size_t index() const noexcept { return m_variant.index(); }

	template< typename T, typename... Args > T &emplace(Args &&... args) {
		return m_variant.template emplace< T >(args...);
	}

	template< typename T, typename U, typename... Args > T &emplace(std::initializer_list< U > il, Args &&... args) {
		return m_variant.template emplace< T >(std::forward(il), args...);
	}

	template< std::size_t I, typename... Args >
	std::variant_alternative_t< I, variant_type > &emplace(Args &&... args) {
		return m_variant.template emplace< I >(args...);
	}

	template< std::size_t I, typename U, typename... Args >
	std::variant_alternative_t< I, variant_type > &emplace(std::initializer_list< U > il, Args &&... args) {
		return m_variant.template emplace< I >(std::forward(il), args...);
	}

	void swap(polymorphic_variant &rhs) { m_variant.swap(rhs); }

private:
	variant_type m_variant;
	Base *m_base_ptr = nullptr;
};

} // namespace pv

#include "pv/free_functions.hpp"

#endif // PV_POLYMORPHICVARIANT_HPP_
