#include <complex>
#include <string>
#include <variant>

template< typename... Types > void *getPtr(std::variant< Types... > &variant) {
	return std::visit([](auto &&value) { return reinterpret_cast< void * >(&value); }, variant);
}

int main() {
	std::variant< int, std::complex< double >, std::string > variant(1);

	void *intStorage = getPtr(variant);

	variant = std::complex< double >(5, 8);

	void *complexStorage = getPtr(variant);

	variant = std::string("Test here");

	void *stringStorage = getPtr(variant);

	if (intStorage == complexStorage && complexStorage == stringStorage) {
		return 0;
	} else {
		return 1;
	}
}
