// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_DETAILS_STORAGE_PTR_HPP__
#define PV_DETAILS_STORAGE_PTR_HPP__

#include <type_traits>
#include <variant>

namespace pv::details {

namespace {
#ifdef PV_USE_SHARED_VARIANT_STORAGE
	static constexpr bool exploit_shared_storage = true;
#else
	static constexpr bool exploit_shared_storage = false;
#endif
} // namespace

template< typename PtrType, typename ActiveType = void, typename... Types > struct storage_ptr {
	static PtrType *get(std::variant< Types... > &variant) {
		if constexpr (exploit_shared_storage && !std::is_same_v< void, ActiveType >) {
			return static_cast< PtrType * >(&std::get< ActiveType >(variant));
		} else {
			return std::visit([](auto &&value) { return static_cast< PtrType * >(&value); }, variant);
		}
	}

	static const PtrType *get(const std::variant< Types... > &variant) {
		if constexpr (exploit_shared_storage && !std::is_same_v< void, ActiveType >) {
			return static_cast< const PtrType * >(&std::get< ActiveType >(variant));
		} else {
			return std::visit([](auto &&value) { return static_cast< const PtrType * >(&value); }, variant);
		}
	}

	static void update(PtrType *&ptr, std::variant< Types... > &variant) {
		if constexpr (exploit_shared_storage) {
			// No update needed
			(void) ptr;
			(void) variant;
		} else {
			ptr = get(variant);
		}
	}

	static void update(const PtrType *&ptr, const std::variant< Types... > &variant) {
		if constexpr (exploit_shared_storage) {
			// No update needed
			(void) ptr;
			(void) variant;
		} else {
			ptr = get(variant);
		}
	}
};

} // namespace pv::details

#endif // PV_DETAILS_STORAGE_PTR_HPP__
