// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_POLYMORPHICVARIANT_IMPL_HPP_
#define PV_POLYMORPHICVARIANT_IMPL_HPP_

#include "pv/details/has_operator.hpp"

#ifndef PV_USE_VISIT_ACCESS
#	include "pv/details/storage_offset.hpp"
#	include "pv/details/variadic_parameter_helper.hpp"
#endif

#include <cassert>
#include <initializer_list>
#include <type_traits>
#include <utility>
#include <variant>

namespace pv::details {

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
	/**
	 * The type of the underlying variant object
	 */
	using variant_type = std::variant< Types... >;

	using base_type = std::decay_t< Base >;


private:
	using self_type = polymorphic_variant< Base, Types... >;

	template< typename T >
	static constexpr bool
		is_wrapped_type = (std::is_same_v< std::remove_cv_t< std::remove_reference_t< T > >, Types > || ...);

	template< typename T > using enable_if_wrapped_type = std::enable_if_t< is_wrapped_type< T > >;

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


	// Constructors
	constexpr polymorphic_variant() = default;

	// TODO: disable depending on copyability/movability of Base
	constexpr polymorphic_variant(const self_type &other) = default;
	// TODO: make move conditional no-except
	constexpr polymorphic_variant(self_type &&other) = default;

	// Constructor taking one of Types
	template< typename T, typename = enable_if_wrapped_type< T > >
	constexpr polymorphic_variant(T &&t)
		: m_variant(std::forward< T >(t))
#ifndef PV_USE_VISIT_ACCESS
		  ,
		  m_base_offset(storage_offset< typename std::decay_t< T >, Types... >::get(m_variant))
#endif
	{
	}

	// Constructor creating one of Types in-place from given arguments
	template< typename T, typename... Args, typename = enable_if_wrapped_type< T > >
	constexpr explicit polymorphic_variant(std::in_place_type_t< T >, Args &&... args)
		: m_variant(std::in_place_type_t< T >{}, std::forward< Args >(args)...)
#ifndef PV_USE_VISIT_ACCESS
		  ,
		  m_base_offset(storage_offset< typename std::decay_t< T >, Types... >::get(m_variant))
#endif
	{
	}

	// Constructor creating one of Types in-place from given initializer list and arguments
	template< typename T, typename U, typename... Args, typename = enable_if_wrapped_type< T > >
	constexpr explicit polymorphic_variant(std::in_place_type_t< T >, std::initializer_list< U > il, Args &&... args)
		: m_variant(std::in_place_type_t< T >{}, il, std::forward< Args >(args)...)
#ifndef PV_USE_VISIT_ACCESS
		  ,
		  m_base_offset(storage_offset< typename std::decay_t< T >, Types... >::get(m_variant))
#endif
	{
	}

	~polymorphic_variant() = default;

	// Custom functions

	/**
	 * Gets the stored value as a base-class reference
	 */
	constexpr Base &get() noexcept {
		assert(!m_variant.valueless_by_exception());
#ifdef PV_USE_VISIT_ACCESS
		return std::visit([](auto &&obj) -> base_type & { return obj; }, m_variant);
#else
		assert(m_base_offset < sizeof(self_type));
		return *reinterpret_cast< base_type * >(reinterpret_cast< unsigned char * >(this) + m_base_offset);
#endif
	}

	/**
	 * Gets the stored value as a base-class reference
	 */
	constexpr const Base &get() const noexcept {
		assert(!m_variant.valueless_by_exception());
#ifdef PV_USE_VISIT_ACCESS
		return std::visit([](auto &&obj) -> const base_type & { return obj; }, m_variant);
#else
		assert(m_base_offset < sizeof(self_type));
		return *reinterpret_cast< const base_type * >(reinterpret_cast< const unsigned char * >(this) + m_base_offset);
#endif
	}

	/**
	 * Accesses the currently stored object via the base-class interface
	 */
	constexpr Base *operator->() noexcept { return &get(); }

	/**
	 * Accesses the currently stored object via the base-class interface
	 */
	constexpr const Base *operator->() const noexcept { return &get(); }


	// TODO: disable depending on copyability/movability of Base
	// Delegating functions for that part of the variant interface that also directly makes sense for
	// polymorphic_variant
	constexpr polymorphic_variant &operator=(const polymorphic_variant &rhs) = default;

	constexpr polymorphic_variant &operator=(polymorphic_variant &&rhs) = default;

	template< typename T, typename = enable_if_wrapped_type< T > > polymorphic_variant &operator=(T &&t) {
		m_variant = std::forward< T >(t);

#ifndef PV_USE_VISIT_ACCESS
		storage_offset< void, Types... >::update(m_base_offset, m_variant);
#endif

		return *this;
	}


	template< typename T, typename... Args, typename = enable_if_wrapped_type< T > > T &emplace(Args &&... args) {
		T &ref = m_variant.template emplace< T >(args...);

#ifndef PV_USE_VISIT_ACCESS
		storage_offset< void, Types... >::update(m_base_offset, m_variant);
#endif

		return ref;
	}

	template< typename T, typename U, typename... Args, typename = enable_if_wrapped_type< T > >
	T &emplace(std::initializer_list< U > il, Args &&... args) {
		T &ref = m_variant.template emplace< T >(std::forward< std::initializer_list< U > >(il), args...);

#ifndef PV_USE_VISIT_ACCESS
		storage_offset< void, Types... >::update(m_base_offset, m_variant);
#endif

		return ref;
	}

	void swap(polymorphic_variant &rhs) {
		m_variant.swap(rhs.m_variant);
#ifndef PV_USE_VISIT_ACCESS
		std::swap(m_base_offset, rhs.m_base_offset);
#endif
	}

	operator base_type &() { return get(); }

	operator const base_type &() const { return get(); }

	template< typename Index, typename = enable_if_has_subscript_t< base_type, Index > > auto operator[](Index &&idx) {
		return get()[std::forward< Index >(idx)];
	}

	template< typename Index, typename = enable_if_has_subscript_t< std::add_const_t< base_type >, Index > >
	auto operator[](Index &&idx) const {
		return get()[std::forward< Index >(idx)];
	}

private:
	variant_type m_variant;
#ifndef PV_USE_VISIT_ACCESS
	std::size_t m_base_offset =
		storage_offset< typename first_variadic_parameter< Types... >::type, Types... >::get(m_variant);
#endif
};

} // namespace pv::details

#endif // PV_POLYMORPHICVARIANT_IMPL_HPP_
