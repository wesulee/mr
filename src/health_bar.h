#pragma once

#include <algorithm>	// max


class Canvas;
class KillableGameEntity;


class HealthBar {
public:
	HealthBar(const KillableGameEntity*, const int);
	virtual ~HealthBar() {}
	virtual void draw(Canvas&) = 0;
	virtual void set(const int) = 0;
	void refresh(void);		// calls set() with current health of entity
	int getHealth(void) const;
	int getMaxHealth(void) const;
	float getRatio(void) const;
protected:
	const KillableGameEntity* entity;
	int health;
	int maxHealth;
};


inline
int HealthBar::getHealth() const {
	return health;
}


inline
int HealthBar::getMaxHealth() const {
	return maxHealth;
}


inline
float HealthBar::getRatio() const {
	return (static_cast<float>(health) / maxHealth);
}
