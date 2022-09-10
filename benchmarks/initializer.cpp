//  Use of this source code is governed by a BSD-style license that can
//  be found in the LICENSE file at the root of the source tree or at
//  <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

#include "initializer.hpp"

pv::polymorphic_variant< Animal, Dog, Cat > initPolyVariant() {
	return pv::polymorphic_variant< Animal, Dog, Cat >(Dog{});
}

pv::polymorphic_variant< Animal, Dog, Cat > initPolyVariant(int arg) {
	return pv::polymorphic_variant< Animal, Dog, Cat >(Dog(arg));
}

std::variant< Dog, Cat > initStdVariant() {
	return std::variant< Dog, Cat >(Dog{});
}

std::variant< Dog, Cat > initStdVariant(int arg) {
	return std::variant< Dog, Cat >(Dog(arg));
}

std::unique_ptr< Animal > initRegular() {
	return std::make_unique< Dog >();
}

std::unique_ptr< Animal > initRegular(int arg) {
	return std::make_unique< Dog >(arg);
}
