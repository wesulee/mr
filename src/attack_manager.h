#pragma once

#include "sdl_header.h"
#include <list>


class Attack;
class Canvas;
class Circle;
class CreatureManager;


class AttackManager {
	AttackManager(const AttackManager&) = delete;
	void operator=(const AttackManager&) = delete;
public:
	AttackManager() = default;
	~AttackManager();
	void update(void);
	void draw(Canvas&);
	void add(Attack*);
	bool procRect(const Attack*, const SDL_Rect&, const float);
	bool procCirc(const Attack*, const Circle&, const float);
	void setCreatureManager(CreatureManager*);
private:
	bool procRectCreatures(const SDL_Rect&, const float);
	bool procCircCreatures(const Circle&, const float);
	std::list<Attack*> list;
	CreatureManager* cm = nullptr;
};
