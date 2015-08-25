#pragma once


enum class Side : unsigned int {NORTH = 0, SOUTH, WEST, EAST};


inline
unsigned int SideToIndex(const Side s) {
	return static_cast<unsigned int>(s);
}
