#pragma once

#include <algorithm>	// copy
#include <cassert>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <vector>


// implement a bit vector for fun
template<std::size_t SIZE>	// size
class BitVector {
public:
	BitVector() {
		std::memset(vec, 0, sizeof(vec));
	}

	~BitVector() = default;

	std::size_t size() const {
		return SIZE;
	}

	bool test(const std::size_t pos) const {
		assert(pos < SIZE);
		const std::size_t index = pos / sizeof(uint8_t);
		const uint8_t test = 1 << (pos - (index * sizeof(uint8_t)));
		return ((test & vec[index]) != 0);
	}

	void set(const std::size_t pos, bool val = true) {
		assert(pos < SIZE);
		const std::size_t index = pos / sizeof(uint8_t);
		uint8_t test;
		if (val) {
			test = 1 << (pos - (index * sizeof(uint8_t)));
			vec[index] |= test;
		}
		else {
			test = ~(1 << (pos - (index * sizeof(uint8_t))));
			vec[index] &= test;
		}
	}

	void set(const std::vector<uint8_t>& arg) {
		assert(arg.size() == (sizeof(vec) / sizeof(vec[0])));
		std::copy(arg.cbegin(), arg.cbegin() + (sizeof(vec) / sizeof(vec[0])), vec);
	}

	std::vector<uint8_t> toVector() const {
		return std::vector<uint8_t>{vec, vec + (sizeof(vec) / sizeof(vec[0]))};
	}
private:
	static constexpr std::size_t getArraySize(const std::size_t typeSize, const std::size_t bits) {
		return ((bits + typeSize - 1) / typeSize);
	}

	char vec[getArraySize(sizeof(uint8_t), SIZE)];
};
