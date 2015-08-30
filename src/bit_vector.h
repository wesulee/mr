#pragma once

#include <algorithm>	// copy
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>


// implement a bit vector for fun
template<std::size_t Size, typename Type=uint8_t>
class BitVector {
public:
	BitVector();
	~BitVector() = default;
	std::size_t size(void) const;
	bool test(const std::size_t) const;
	void set(const std::size_t, const bool = true);
	void set(const std::vector<Type>&);
	std::vector<Type> toVector() const;
private:
	static constexpr Type getTestVal(const std::size_t, const std::size_t);
	static std::size_t getIndex(const std::size_t);
	static constexpr std::size_t getArraySize(const std::size_t, const std::size_t);

	char vec[getArraySize(sizeof(Type), Size)];
};


template<std::size_t Size, typename Type>
BitVector<Size, Type>::BitVector() {
	std::memset(vec, 0, sizeof(vec));
}


template<std::size_t Size, typename Type>
std::size_t BitVector<Size, Type>::size() const {
	return Size;
}


template<std::size_t Size, typename Type>
bool BitVector<Size, Type>::test(const std::size_t pos) const {
	const std::size_t index = getIndex(pos);
	const Type t = getTestVal(pos, index);
	return ((t & vec[index]) != 0);
}


template<std::size_t Size, typename Type>
void BitVector<Size, Type>::set(const std::size_t pos, const bool val) {
	const std::size_t index = getIndex(pos);
	Type t;
	if (val) {
		t = getTestVal(pos, index);
		vec[index] |= t;
	}
	else {
		t = ~getTestVal(pos, index);
		vec[index] &= t;
	}
}


template<std::size_t Size, typename Type>
void BitVector<Size, Type>::set(const std::vector<Type>& arg) {
	assert(arg.size() == (sizeof(vec) / sizeof(vec[0])));
	std::copy(arg.cbegin(), arg.cbegin() + (sizeof(vec) / sizeof(vec[0])), vec);
}


template<std::size_t Size, typename Type>
std::vector<Type> BitVector<Size, Type>::toVector() const {
	return std::vector<uint8_t>{vec, vec + (sizeof(vec) / sizeof(vec[0]))};
}


template<std::size_t Size, typename Type>
constexpr Type BitVector<Size, Type>::getTestVal(const std::size_t pos, const std::size_t idx) {
	return (1 << (pos - (idx * sizeof(Type))));
}


template<std::size_t Size, typename Type>
std::size_t BitVector<Size, Type>::getIndex(const std::size_t pos) {
	assert(pos < Size);
	return (pos / sizeof(Type));
}


template<std::size_t Size, typename Type>
constexpr std::size_t BitVector<Size, Type>::getArraySize(const std::size_t typeSize, const std::size_t bits) {
	return ((bits + typeSize - 1) / typeSize);	// ceil(bits / typeSize)
}
