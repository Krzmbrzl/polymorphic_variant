// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_DETAILS_STORAGE_PTR_HPP__
#define PV_DETAILS_STORAGE_PTR_HPP__

#include <cassert>
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

template< typename ActiveType = void, typename... Types > struct storage_offset {
	static std::size_t get(const std::variant< Types... > &variant) {
		std::ptrdiff_t offset;
		if constexpr (exploit_shared_storage && !std::is_same_v< void, ActiveType >) {
			offset = reinterpret_cast< const unsigned char * >(&std::get< ActiveType >(variant))
					 - reinterpret_cast< const unsigned char * >(&variant);
		} else {
			offset = reinterpret_cast< const unsigned char * >(std::visit([](auto &&value) { return &value; }, variant))
					 - reinterpret_cast< const unsigned char * >(&variant);
		}

		assert(offset >= 0);

		return static_cast< std::size_t >(offset);
	}

	static std::size_t get(std::variant< Types... > &variant) {
		return get(const_cast< const std::variant< Types... > & >(variant));
	}

	static void update(std::size_t &offset, std::variant< Types... > &variant) {
		if constexpr (exploit_shared_storage) {
			// No update needed
			(void) offset;
			(void) variant;
		} else {
			offset = get(variant);
		}
	}
};

} // namespace pv::details

#endif // PV_DETAILS_STORAGE_PTR_HPP__
