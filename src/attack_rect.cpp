#include "attack_rect.h"
#include "attack_manager.h"
#include "canvas.h"
#include "game_data.h"
#include "main_game_objects.h"


AttackRect::AttackRect() : rect({0, 0, 0, 0}), damage(0) {
}


AttackRect::AttackRect(const SDL_Rect& r, const int d) : rect(r), damage(d) {
}


bool AttackRect::update(const Constants::float_type) {
	GameData::instance().mgo->getAttackManager().procRect(this, rect, damage);
	return true;
}


void AttackRect::draw(Canvas& can) {
	(void)can;
}


void AttackRect::setRect(const SDL_Rect& r) {
	rect = r;
}


void AttackRect::setDamage(const int d) {
	damage = d;
}
