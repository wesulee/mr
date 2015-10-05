#pragma once

#include "entity.h"
#include "utility.h"


class CreatureManager;
class HealthBar;


// Derived Creature may construct their own health bar during ctor if they don't want default one.
//   Should not interact with healthBar otherwise.
class Creature : public KillableGameEntity {
public:
	Creature(const int);
	virtual ~Creature();
	virtual void spawn(CreatureManager*, const int, const int) = 0;
	void updatePos(const float, const float) override;
	void setPos(const float, const float) override;
	void damage(const int) override;
	HealthBar*& getHealthBar(void);
protected:
	Vector2D<float> dpos;
	HealthBar* healthBar = nullptr;
};


inline
Creature::Creature(const int hp) : KillableGameEntity(hp) {
}


inline
Creature::~Creature() {
	// Do nothing.
	// healthBar must be deleted by CreatureManager
}


inline
void Creature::updatePos(const float x, const float y) {
	entityPos.x += x;
	entityPos.y += y;
}


inline
void Creature::setPos(const float x, const float y) {
	entityPos.x = x;
	entityPos.y = y;
}


inline
void Creature::damage(const int d) {
	decHealth(d);
}


inline
HealthBar*& Creature::getHealthBar() {
	return healthBar;
}
