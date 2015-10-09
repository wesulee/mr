#pragma once

#include <cstdint>
#include <vector>


struct SaveData {
	std::vector<uint8_t> mapVec;
	int roomX;
	int roomY;
	// player
	float posX;
	float posY;
	int health;

	template<class Archive>
	void serialize(Archive& ar, const unsigned int) {
		ar & mapVec;
		ar & roomX;
		ar & roomY;
		ar & posX;
		ar & posY;
		ar & health;;
	}
};
