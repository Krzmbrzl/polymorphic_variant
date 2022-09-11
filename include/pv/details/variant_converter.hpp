// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_DETAILS_VARIANTTYPE_HPP_
#define PV_DETAILS_VARIANTTYPE_HPP_

#include "pv/details/is_variant.hpp"
#include "pv/polymorphic_variant.hpp"

#include <type_traits>
#include <variant>

namespace pv {
namespace details {

	template< typename Variant > struct variant_converter {
		using variant_type = std::remove_reference_t< Variant >;

		static_assert(is_variant_v< Variant >, "Can only feed variant types into variant_converter");

		static variant_type &toVariant(variant_type &var) { return var; }

		static const variant_type &toVariant(const variant_type &var) { return var; }

		static variant_type *variant_ptr(variant_type *ptr) { return ptr; }

		static const variant_type *variant_ptr(const variant_type *ptr) { return ptr; }
	};

	template< typename Base, typename... Types >
	struct variant_converter<::pv::polymorphic_variant< Base, Types... > > {
		using variant_type = typename ::pv::polymorphic_variant< Base, Types... >::variant_type;

		static variant_type &toVariant(::pv::polymorphic_variant< Base, Types... > &var) { return var.variant(); }

		static const variant_type &toVariant(const ::pv::polymorphic_variant< Base, Types... > &var) {
			return var.variant();
		}

		static variant_type *variant_ptr(::pv::polymorphic_variant< Base, Types... > *ptr) {
			return ptr ? &ptr->variant() : nullptr;
		}

		static const variant_type *variant_ptr(const ::pv::polymorphic_variant< Base, Types... > *ptr) {
			return ptr ? &ptr->variant() : nullptr;
		}
	};

	// Use the same specialization for references as for values
	template< typename Base, typename... Types >
	struct variant_converter<::pv::polymorphic_variant< Base, Types... > & >
		: variant_converter<::pv::polymorphic_variant< Base, Types... > > {};

}; // namespace details
} // namespace pv

#endif // PV_DETAILS_VARIANTTYPE_HPP_
