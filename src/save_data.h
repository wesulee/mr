#pragma once

#include <cstdint>
#include <vector>


struct SaveData {
	std::vector<uint8_t> mapVec;
	int roomX;
	int roomY;
	float posX;
	float posY;
	float pHealth;	// player health

	template<class Archive>
	void serialize(Archive& ar, const unsigned int ver) {
		(void)ver;
		ar & mapVec;
		ar & posX& posY;
		ar & roomX;
		ar & roomY;
		ar & pHealth;;
	}
};
