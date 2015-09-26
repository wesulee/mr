#pragma once

#include "creature_type.h"
#include "json_reader.h"
#include "sdl_header.h"
#include "utility.h"
#include <list>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>


class Canvas;
class Circle;
class Creature;
class KillableGameEntity;
class Player;
class Room;
class Sprite;


class CreatureManager {
	CreatureManager(const CreatureManager&) = delete;
	void operator=(const CreatureManager&) = delete;
	enum class CResourceType {ANIMATION, SPRITESHEET};
	struct CreatureResources {
		std::string name;
		CResourceType type;
	};
	typedef std::vector<CreatureResources> ResourceList;
	typedef std::unordered_set<CreatureType, EnumClassHash> CreatureTypeSet;
	typedef std::unordered_map<CreatureType, ResourceList, EnumClassHash> CreatureResourceMap;
public:
	CreatureManager(Player*, Room*);
	~CreatureManager();
	void update(void);
	void draw(Canvas&);
	KillableGameEntity* getTarget(void);
	Room* getRoom(void);
	Sprite getSprite(const std::string&);
	Player* getPlayer(void) const;
	bool spawn(const CreatureType, const int, const int);	// called by Creature
	std::vector<Creature*> getRect(const SDL_Rect&) const;	// get creatures contained in the rect
	bool intersectsPlayer(const SDL_Rect&) const;
	bool intersectsPlayer(const Circle&) const;
	void setRoom(rapidjson::Document&);
private:
	void loadCreature(const CreatureType);
	void unloadCreature(const CreatureType);
	void loadAnimations(rapidjson::Document&, ResourceList&);
	CreatureType getCreatureType(const std::string&);
	void notifyEmpty(void) const;
	static Creature* newCreature(const CreatureType);

	std::unordered_map<std::string, CreatureType> lookupMap;
	CreatureResourceMap loaded;
	std::list<Creature*> creatures;
	Player* player = nullptr;
	Room* room = nullptr;
};


inline
Room* CreatureManager::getRoom() {
	return room;
}


inline
Player* CreatureManager::getPlayer() const {
	return player;
}
