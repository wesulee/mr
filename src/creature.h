#pragma once

#include "constants.h"
#include "entity.h"
#include "health_bar.h"
#include "utility.h"


class Canvas;
class CreatureManager;


class Creature : public KillableGameEntity {
public:
	Creature(const int);
	virtual ~Creature() {}
	virtual void spawn(CreatureManager*, const int, const int) = 0;
	float getPosX(void) const override;
	float getPosY(void) const override;
	void updatePos(const float, const float) override;
	void setPos(const float, const float) override;
	int getHealth(void) const override;
	void damage(const int) override;
	bool isAlive(void) const override;
protected:
	void crUpdate(void);
	bool shouldDrawHPBar(void) const;

	CreatureHealthBar healthBar;
	Vector2D<float> pos;
	Vector2D<float> dpos;
	Counter hpCounter;
};


inline
Creature::Creature(const int hp) : healthBar(hp), hpCounter(Constants::CrHPBarTicks) {
	hpCounter.setTicks(Constants::CrHPBarTicks);	// initially not drawn
}


inline
float Creature::getPosX() const {
	return pos.x;
}


inline
float Creature::getPosY() const {
	return pos.y;
}


inline
void Creature::updatePos(const float x, const float y) {
	pos.x += x;
	pos.y += y;
}


inline
void Creature::setPos(const float x, const float y) {
	pos.x = x;
	pos.y = y;
}


inline
int Creature::getHealth() const {
	return healthBar.getHealth();
}


inline
void Creature::damage(const int d) {
	healthBar.damage(d);
	hpCounter.reset();
}


inline
bool Creature::isAlive() const {
	return healthBar.isAlive();
}


inline
void Creature::crUpdate() {
	hpCounter.increment();
}


inline
bool Creature::shouldDrawHPBar() const {
	return !hpCounter.finished();
}
