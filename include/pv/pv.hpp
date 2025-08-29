// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#ifndef PV_PV_HPP_
#define PV_PV_HPP_

#include "pv/details/polymorphic_variant_impl.hpp"
#include "pv/details/operators_impl.hpp"

namespace pv {

inline namespace v2 {
	template<typename Base, typename...Types>
	using pv = details::polymorphic_variant<Base, Types...>;

	using details::polymorphic_variant;
}

}

#endif // PV_PV_HPP_
