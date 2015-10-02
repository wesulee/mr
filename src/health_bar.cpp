#include "health_bar.h"
#include "entity.h"


HealthBar::HealthBar(const KillableGameEntity* e, const int hp) : entity(e), health(e->getHealth()), maxHealth(hp) {
}


void HealthBar::refresh() {
	const int newHealth = entity->getHealth();
	if (health != newHealth)
		set(newHealth);
}
