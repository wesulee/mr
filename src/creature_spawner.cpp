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


void CreatureSpawner::updateTick() {
	counter.increment();
	if (counter.finished()) {
		counter.reset();
		setSpawnCounter();
		const int spawnCount = distSpawnN(GameData::instance().randGen);
		for (int i = 0; i < spawnCount; ++i) {
			//! TODO: check result, randomize position
			cm->spawn(
				spawnType,
				static_cast<int>(pos.x),
				static_cast<int>(pos.y)
			);
		}
	}
}


void CreatureSpawner::setSpawnCounter() {
	const int timeMS = distSpawnMS(GameData::instance().randGen);
	counter.setMaxTicks(static_cast<unsigned int>(timeMS / Constants::frameDurationFloat));
}
