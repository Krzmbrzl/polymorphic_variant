// Use of this source code is governed by a BSD-style license that can
// be found in the LICENSE file at the root of the source tree or at
// <https://github.com/Krzmbrzl/polymorphic_variant/blob/main/LICENSE>.

class Base {
public:
	Base()          = default;
	virtual ~Base() = default;

	Base &operator*=(const Base &other) {
		multiply(other.factor());

		return *this;
	}

	int operator*(const Base &other) const { return result() * other.factor(); }

	// prefix increment
	Base &operator++() {
		setResult(result() + 1);

		return *this;
	}

	virtual void multiply(int factor) = 0;

	virtual int factor() const = 0;

	virtual int result() const = 0;

	virtual void setResult(int result) = 0;
};

inline double operator/(const Base &lhs, const Base &rhs) {
	return lhs.result() / static_cast< double >(rhs.factor());
}

class Derived1 : public Base {
public:
	static constexpr const int InitialValue = 1;
	static constexpr const int Factor       = 42;

	void multiply(int factor) override { m_result *= factor; }

	int factor() const override { return m_factor; }

	int result() const override { return m_result; }

	void setResult(int result) override { m_result = result; }

protected:
	int m_result = InitialValue;
	int m_factor = Factor;
};

class Derived2 : public Base {
public:
	static constexpr const int InitialValue = -20;
	static constexpr const int Factor       = -5;

	void multiply(int factor) override { m_result *= factor; }

	int factor() const override { return m_factor; }

	int result() const override { return m_result; }

	void setResult(int result) override { m_result = result; }

protected:
	int m_result = InitialValue;
	int m_factor = Factor;
};
