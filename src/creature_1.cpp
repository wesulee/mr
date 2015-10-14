#include "creature_1.h"
#include "attack_manager.h"
#include "attack_rect.h"
#include "canvas.h"
#include "constants.h"
#include "creature_manager.h"
#include "game_data.h"
#include "main_game_objects.h"
#include "resource_manager.h"
#include "room.h"
#include <cmath>
#include <cassert>


namespace Creature1Settings {
	constexpr int maxAttackTicks = 16;
	// Minimum square distance from self to target to initiate attack.
	constexpr float minDistAttack2 = 7 * 7;
	constexpr float speed = 50;	// pixels per second, includes both axes
	constexpr unsigned int refreshTargetDirection = 5;
	constexpr int attackWidth = 10;
	constexpr int attackHeight = 20;
}


Creature1::Creature1() : Creature(100), attackRect({0, 0, 8, 14}) {
	auto source = GameData::instance().resources->getUSprSrc("sk_mv_l");
	assert(source != nullptr);
	sprMovL.setSource(source);
	source = GameData::instance().resources->getUSprSrc("sk_mv_r");
	assert(source != nullptr);
	sprMovR.setSource(source);
}


Creature1::~Creature1() {
	// do nothing
}


void Creature1::spawn(CreatureManager* cm, const int x, const int y) {
	target = cm->getTarget();
	entityPos.x = x;
	entityPos.y = y;

	state = CreatureState::MOVING;
	updateTargetPos();
}


bool Creature1::update() {
	using namespace Creature1Settings;
	counter.increment();
	switch (state) {
	case CreatureState::NONE:
		break;
	case CreatureState::MOVING:
		movingUpdate();
		break;
	case CreatureState::ATTACKING:
		if (++attackTicks >= maxAttackTicks) {
			state = CreatureState::MOVING;
		}
		break;
	case CreatureState::KNOCKBACK:
		//! TODO not implemented
		break;
	}
	return false;
}


void Creature1::draw(Canvas& can) {
	can.draw(*curSpr, static_cast<int>(entityPos.x), static_cast<int>(entityPos.y));
#ifndef NDEBUG
	if (state == CreatureState::ATTACKING) {
		auto oldColor = can.getColorState();
		can.setColor(COLOR_RED, SDL_ALPHA_OPAQUE / 2);
		can.fillRect(attackRect);
		can.setColorState(oldColor);
	}
#endif // NDEBUG
}


SDL_Rect Creature1::getBounds() const {
	return {static_cast<int>(entityPos.x), static_cast<int>(entityPos.y), sprMovL.getDrawWidth(), sprMovL.getDrawHeight()};
}


bool Creature1::shouldAttack() {
	using namespace Creature1Settings;
	SDL_Rect rect = target->getBounds();
	if (facingLeft()) {
		return (
			square(rect.x + rect.w - entityPos.x)
			+ square(rect.y - entityPos.y)
			< minDistAttack2
		);
	}
	else {
		return (
			square(entityPos.x + sprMovL.getDrawWidth() - rect.x)
			+ square(rect.y - entityPos.y)
			< minDistAttack2
		);
	}
}


// recalculate the target position
void Creature1::updateTargetPos() {
	using namespace Creature1Settings;
	SDL_Rect tRect = target->getBounds();
	const float selfCenterX = entityPos.x + sprMovL.getDrawWidth() / 2.0f;
	const float targetCenterX = tRect.x + tRect.w / 2.0f;
	float targetX;
	if (selfCenterX <= targetCenterX) {
		// current to the left of target
		curSpr = &sprMovR;
		targetX = tRect.x - sprMovL.getDrawWidth();
	}
	else {
		curSpr = &sprMovL;
		targetX = tRect.x + tRect.w;
	}
	// update direction vector
	dpos.x = targetX - entityPos.x;
	dpos.y = tRect.y - entityPos.y;
	dpos.normalize();
	dpos *= (speed / 1000 * Constants::frameDurationFloat);
}


void Creature1::updatePosition() {
	curSpr->update();
	GameData::instance().mgo->getRoom().update(*this, dpos);
}


// is the current direction the sprite is facing left?
bool Creature1::facingLeft() const {
	return (curSpr == &sprMovL);
}


void Creature1::movingUpdate() {
	using namespace Creature1Settings;
	if (shouldAttack()) {
		attackTicks = 0;
		state = CreatureState::ATTACKING;
		// set attack rectangle
		if (facingLeft()) {
			attackRect.x = static_cast<int>(entityPos.x) - attackRect.w;
			attackRect.y = static_cast<int>(entityPos.y);
		}
		else {
			attackRect.x = static_cast<int>(entityPos.x) + sprMovL.getDrawWidth();
			attackRect.y = static_cast<int>(entityPos.y);
		}

		AttackRect* ar = new AttackRect;
		ar->setSource(this);
		ar->setTarget(AttackTarget::PLAYER);
		ar->setRect(attackRect);
		ar->setDamage(1);
		GameData::instance().mgo->getAttackManager().add(ar);
	}
	else {
		updatePosition();
		if (counter.getTicks() % refreshTargetDirection == 0)
			updateTargetPos();
	}
}
