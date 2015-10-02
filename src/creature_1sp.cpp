#include "creature_1sp.h"
#include "canvas.h"
#include "creature_manager.h"


Creature1Sp::Creature1Sp(): CreatureSpawner(100, CreatureType::SP1, CreatureType::T1, 10 * 1000, 30 * 1000, 3, 5) {
}


void Creature1Sp::spawn(CreatureManager* man, const int x, const int y) {
	cm = man;
	pos.x = x;
	pos.y = y;
	sp = cm->getSprite("cr1sp");
}


bool Creature1Sp::update() {
	updateTick();
	return false;
}


void Creature1Sp::draw(Canvas& can) {
	can.draw(sp, static_cast<int>(pos.x), static_cast<int>(pos.y));
}


SDL_Rect Creature1Sp::getBounds() const {
	return {static_cast<int>(pos.x), static_cast<int>(pos.y), sp.getDrawWidth(), sp.getDrawHeight()};
}
