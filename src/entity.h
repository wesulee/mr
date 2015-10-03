#pragma once

#include "utility.h"	// Vector2D
#include <algorithm>	// max


class Canvas;
class SDL_Rect;


// Represents something abstract...
class Entity {
public:
	Entity() {}
	virtual ~Entity() {}
	virtual bool update(void) = 0;
	virtual void draw(Canvas&) = 0;
};


class GameEntity : public Entity {
public:
	GameEntity() {}
	virtual ~GameEntity() {}
	virtual SDL_Rect getBounds(void) const = 0;
	Vector2D<> getPos(void) const;
	virtual void updatePos(const float, const float) = 0;
	virtual void setPos(const float, const float) = 0;
protected:
	Vector2D<> entityPos;
};


class KillableGameEntity : public GameEntity {
public:
	virtual int getHealth(void) const = 0;
	virtual void damage(const int) = 0;
protected:
	static int decHealth(const int, const int);
};


inline
int KillableGameEntity::decHealth(const int curHealth, const int damage) {
	return std::max(curHealth - damage, 0);
}


inline
Vector2D<> GameEntity::getPos() const {
	return entityPos;
}
