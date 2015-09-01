#include "creature_manager.h"
#include "canvas.h"
#include "creature.h"
#include "exception.h"
#include "game_data.h"
#include "json_data.h"
#include "parameters.h"
#include "player.h"
#include "resource_manager.h"
#include "room.h"
#include "sdl_header.h"
#include "shapes.h"
#ifndef NDEBUG
#include "constants.h"
#endif
// All creatures
#include "creature_1.h"
#include "creature_1sp.h"


CreatureManager::CreatureManager(Player* p, Room* r) : player(p), room(r) {
	// populate creature string to type mapping
	lookupMap.emplace("cr1", CreatureType::T1);
	lookupMap.emplace("cr1sp", CreatureType::SP1);
}


CreatureManager::~CreatureManager() {
	for (auto c : creatures)
		delete c;
}


void CreatureManager::update() {
	if (creatures.empty())
		notifyEmpty();
	for (auto it = creatures.begin(); it != creatures.end();) {
		if (!(**it).isAlive() || (**it).update()) {
			delete *it;
			it = creatures.erase(it);
		}
		else {
			++it;
		}
	}
}


void CreatureManager::draw(Canvas& can) {
	for (auto c : creatures) {
		c->draw(can);
#if defined(DEBUG_CREATURE_BOUNDS) && DEBUG_CREATURE_BOUNDS
		// draw overlay over each creature
		auto rect = c->getBounds();
		auto oldColor = can.getColorState();
		can.setColor(DEBUG_CREATURE_BOUNDS_COLOR, getAlpha<DEBUG_CREATURE_BOUNDS_ALPHA>());
		can.fillRect(rect);
		can.setColorState(oldColor);
#endif // DEBUG_CREATURE_BOUNDS
	}
}


Sprite CreatureManager::getSprite(const std::string& name) {
	return GameData::instance().resources->getSprite(name);
}


KillableGameEntity* CreatureManager::getTarget() {
	return player;
}


bool CreatureManager::spawn(const CreatureType ct, const int x, const int y) {
	Creature* c = newCreature(ct);
	if (c == nullptr) {
		return false;
	}
	c->spawn(this, x, y);
	//! TODO make sure creature has spawned in valid location (not inside room block)
	creatures.push_back(c);
	return true;
}


std::vector<Creature*> CreatureManager::getRect(const SDL_Rect& rect) const {
	std::vector<Creature*> vc;
	for (auto c : creatures) {
		if (Shape::intersects(rect, c->getBounds()))
			vc.push_back(c);
	}
	return vc;
}


bool CreatureManager::intersectsPlayer(const SDL_Rect& rect) const {
	return Shape::intersects(rect, player->getBounds());
}


bool CreatureManager::intersectsPlayer(const Circle& circ) const {
	return Shape::intersects(player->getBounds(), circ);
}


void CreatureManager::loadRoom(std::shared_ptr<RoomData> rd) {
	// process loading/unloading creatures
	// first populate creatures used in room
	CreatureTypeSet roomCreatures;
	for (auto& cd : rd->creatures) {
		cd.type = getCreatureType(cd.name);
		if (cd.type == CreatureType::NONE) {
			logAndExit(RuntimeError{
				"error loading room",
				"CreatureManager::loadRoom creature \"" + cd.name + "\" invalid"
			});
		}
		roomCreatures.insert(cd.type);
	}
	// find creatures to unload
	for (auto it = loaded.cbegin(); it != loaded.cend();) {
		if (roomCreatures.count(it->first) == 0) {
			unloadCreature(it->first);
			it = loaded.erase(it);
		}
		else
			++it;
	}
	// find creatures to load
	for (const auto ct : roomCreatures) {
		if (loaded.count(ct) == 0) {
			loadCreature(ct);
		}
	}
	// spawn creatures
	for (const auto& cd : rd->creatures)
		spawn(cd.type, cd.x, cd.y);
}


// Note: DOES update member loaded
void CreatureManager::loadCreature(const CreatureType ct) {
	assert(loaded.count(ct) == 0);
	auto& list = loaded[ct];	// insert and get default object
	// find name associated with CreatureType
	auto it = lookupMap.cbegin();
	for (; it != lookupMap.cend(); ++it) {
		if (it->second == ct)
			break;
	}
	assert(it != lookupMap.cend());
	// load resources
	std::shared_ptr<CreatureData> data = GameData::instance().resources->getCreatureData(it->first);
	loadAnimations(*data, list);
}


// unloads all resources used by a Creature
// Note: does not update member loaded
void CreatureManager::unloadCreature(const CreatureType ct) {
	auto lit = loaded.at(ct);
	for (auto it = lit.cbegin(); it != lit.cend(); ++it) {
		switch (it->type) {
		case CResourceType::ANIMATION:
			GameData::instance().resources->freeAnimation(it->name);
			break;
		case CResourceType::SPRITESHEET:
			GameData::instance().resources->freeSpriteSheet(it->name);
			break;
		}
	}
}


void CreatureManager::loadAnimations(const CreatureData& data, ResourceList& list) {
	if (data.animations.empty())
		return;
	for (auto it = data.animations.cbegin(); it != data.animations.cend(); ++it) {
		GameData::instance().resources->loadAnimation(*it);
		list.push_back({it->at("name"), CResourceType::ANIMATION});
	}
}


CreatureType CreatureManager::getCreatureType(const std::string& name) {
	const auto exists = lookupMap.find(name);
	if (exists == lookupMap.end())
		return CreatureType::NONE;
	else
		return exists->second;
}


// push a SDL_Event to notify empty
void CreatureManager::notifyEmpty() const {
	SDL_Event e;
	e.user.code = Parameters::CREATURES_EMPTY;
	if (!SDL::pushUserEvent(e)) {
		//! record warning
		//SDL::logError("CreatureManager::notifyEmpty SDL::pushUserEvent");
		//throw std::runtime_error("SDL event failure");
	}
}


Creature* CreatureManager::newCreature(const CreatureType ct) {
	assert(ct != CreatureType::NONE);
	Creature* c = nullptr;
	switch (ct) {
	case CreatureType::NONE:
		break;
	case CreatureType::T1:
		c = new Creature1;
		break;
	case CreatureType::SP1:
		c = new Creature1Sp;
		break;
	}
	return c;
}
