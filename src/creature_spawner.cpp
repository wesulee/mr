#include "creature_spawner.h"
#include "constants.h"
#include "creature_manager.h"
#include "creature_type.h"
#include "game_data.h"
#include <cassert>


CreatureSpawner::CreatureSpawner(const int hp, const CreatureType tType, const CreatureType sType,
const int msLo, const int msHi, const int nLo, const int nHi)
: Creature(hp), distSpawnMS(msLo, msHi), distSpawnN(nLo, nHi), thisType(tType), spawnType(sType) {
	assert((msLo >= 0) && (msHi >= msLo));
	assert((nLo >= 1) && (nHi >= nLo));
	setSpawnCounter();
}


void CreatureSpawner::updateTick(const Constants::float_type dt) {
	timeRem -= dt;
	if (timeRem <= 0) {
		setSpawnCounter();
		const int spawnCount = distSpawnN(GameData::instance().randGen);
		for (int i = 0; i < spawnCount; ++i) {
			//! TODO: check result, randomize position
			cm->spawn(
				spawnType,
				static_cast<int>(entityPos.x),
				static_cast<int>(entityPos.y)
			);
		}
	}
}


void CreatureSpawner::setSpawnCounter() {
	const int timeMS = distSpawnMS(GameData::instance().randGen);
	timeRem = (static_cast<Constants::float_type>(timeMS) / 1000);
}
