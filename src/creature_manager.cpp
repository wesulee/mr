#include "creature_manager.h"
#include "canvas.h"
#include "creature.h"
#include "game_data.h"
#include "logger.h"
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


// room data
void CreatureManager::setRoom(rapidjson::Document& data) {
	namespace rj = rapidjson;
	// process loading/unloading creatures
	// first populate creatures used in room
	std::vector<CreatureType> dataCrTypes;
	const rj::Value& crData = data["creatures"];
	dataCrTypes.reserve(crData.Size());
	for (rj::Value::ConstValueIterator it = crData.Begin(); it != crData.End(); ++it) {
		const CreatureType cType = getCreatureType((*it)["name"].GetString());
		assert(cType != CreatureType::NONE);
		dataCrTypes.push_back(cType);
	}
	// convert to set for fast set operations
	CreatureTypeSet roomCreatures{dataCrTypes.cbegin(), dataCrTypes.cend()};
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
	std::size_t i = 0;
	int x;
	int y;
	for (rj::Value::ConstValueIterator it = crData.Begin(); it != crData.End(); ++it, ++i) {
		x = (*it)["x"].GetInt();
		y = (*it)["y"].GetInt();
		spawn(dataCrTypes[i], x, y);
	}
}


// Note: DOES update member loaded
void CreatureManager::loadCreature(const CreatureType ct) {
	assert(loaded.count(ct) == 0);	// should not already be loaded
	auto& list = loaded[ct];	// insert and get default object
	// find name associated with CreatureType
	auto it = lookupMap.cbegin();
	for (; it != lookupMap.cend(); ++it) {
		if (it->second == ct)
			break;
	}
	assert(it != lookupMap.cend());
	// load
	std::shared_ptr<rapidjson::Document> data = GameData::instance().resources->getCreatureData(it->first);
	//! TODO process attr
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
			GameData::instance().resources->freeSpriteSheet(it->name, false, true);
			break;
		}
	}
}


void CreatureManager::loadAnimations(rapidjson::Document& data, ResourceList& list) {
	namespace rj = rapidjson;
	if (!data.HasMember("animations"))
		return;
	CreatureResources crRes;
	crRes.type = CResourceType::ANIMATION;
	const rj::Value& animations = data["animations"];
	for (rj::Value::ConstValueIterator it = animations.Begin(); it != animations.End(); ++it) {
		GameData::instance().resources->loadAnimation(*it);
		crRes.name = (*it)["name"].GetString();
		list.push_back(crRes);
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
