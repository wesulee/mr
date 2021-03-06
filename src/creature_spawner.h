#pragma once

#include "creature.h"
#include "creature_type.h"
#include "utility.h"
#include <random>


class CreatureSpawner : public Creature {
public:
	CreatureSpawner(const int, const CreatureType, const CreatureType, const int, const int, const int, const int);
	virtual ~CreatureSpawner() {}
	virtual void spawn(CreatureManager*, const int, const int) = 0;
protected:
	void updateTick(const Constants::float_type);
private:
	void setSpawnCounter(void);
protected:
	CreatureManager* cm = nullptr;
	std::uniform_int_distribution<int> distSpawnMS;		// duration (ms) between spawns
	std::uniform_int_distribution<int> distSpawnN;		// number spawned
	Constants::float_type timeRem = 0;
	CreatureType thisType = CreatureType::NONE;
	CreatureType spawnType = CreatureType::NONE;
};
