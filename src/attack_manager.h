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
	bool procRect(const Attack*, const SDL_Rect&, const int);
	bool procCirc(const Attack*, const Circle&, const int);
	void setCreatureManager(CreatureManager*);
private:
	bool procRectCreatures(const SDL_Rect&, const int);
	bool procCircCreatures(const Circle&, const int);

	std::list<Attack*> list;
	CreatureManager* cm = nullptr;
};
