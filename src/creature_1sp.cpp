#include "creature_1sp.h"
#include "canvas.h"
#include "creature_manager.h"


Creature1Sp::Creature1Sp(): CreatureSpawner(100, CreatureType::SP1, CreatureType::T1, 10 * 1000, 30 * 1000, 3, 5) {
}


void Creature1Sp::spawn(CreatureManager* man, const int x, const int y) {
	cm = man;
	entityPos.x = x;
	entityPos.y = y;
	sp = cm->getSprite("cr1sp");
}


bool Creature1Sp::update(const Constants::float_type dt) {
	updateTick(dt);
	return false;
}


void Creature1Sp::draw(Canvas& can) {
	can.draw(sp, static_cast<int>(entityPos.x), static_cast<int>(entityPos.y));
}


SDL_Rect Creature1Sp::getBounds() const {
	return {static_cast<int>(entityPos.x), static_cast<int>(entityPos.y), sp.getDrawWidth(), sp.getDrawHeight()};
}
