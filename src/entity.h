#pragma once

#include "utility.h"	// Vector2D
#include <algorithm>	// max
#include <cassert>


class Canvas;
class EntityResource;
class SDL_Rect;


// Represents something that can be seen.
// An entity may require resources (sprites, fonts, etc), in which case it
//   may be useful to override loadResource() and unloadResource().
// loadResource() returns a new instance of the resources the entity requires.
// unloadResource() frees the resources acquired from loadResource().
// loadResource() and unloadResource() should never be called directly by the entity.
class Entity {
public:
	Entity() {}
	virtual ~Entity() {}
	virtual bool update(void) = 0;
	virtual void draw(Canvas&) = 0;
	virtual EntityResource* loadResource(void);
	virtual void unloadResource(EntityResource*);
};


// Represents something that has a defined position
class GameEntity : public Entity {
public:
	GameEntity() {}
	virtual ~GameEntity() {}
	virtual SDL_Rect getBounds(void) const = 0;
	Vector2D<> getPos(void) const;
	virtual void updatePos(const float, const float) = 0;
	virtual void setPos(const Vector2D<>&);
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
EntityResource* Entity::loadResource() {
	assert(false);
	return nullptr;
}


inline
void Entity::unloadResource(EntityResource*) {
	assert(false);
}


inline
Vector2D<> GameEntity::getPos() const {
	return entityPos;
}


inline
void GameEntity::setPos(const Vector2D<>& pos) {
	entityPos = pos;
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
