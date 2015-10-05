#pragma once

#include "utility.h"	// Vector2D
#include <algorithm>	// max


class Canvas;
class SDL_Rect;


// Represents something that can be seen
class Entity {
public:
	Entity() {}
	virtual ~Entity() {}
	virtual bool update(void) = 0;
	virtual void draw(Canvas&) = 0;
};


// Represents something that has a defined position
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
	KillableGameEntity(const int);
	virtual ~KillableGameEntity() {}
	int getHealth(void) const;
	virtual void damage(const int) = 0;
protected:
	void decHealth(const int);

	int entityHealth;
};


inline
Vector2D<> GameEntity::getPos() const {
	return entityPos;
}


inline
KillableGameEntity::KillableGameEntity(const int hp) : entityHealth(hp) {
}


inline
int KillableGameEntity::getHealth() const {
	return entityHealth;
}


inline
void KillableGameEntity::decHealth(const int damage) {
	entityHealth = std::max(entityHealth - damage, 0);
}
