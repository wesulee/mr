#include "attack_manager.h"
#include "attack.h"
#include "canvas.h"
#include "creature.h"
#include "creature_manager.h"
#include "player.h"
#include "shapes.h"
#include <cassert>


static void damage(Creature& c, const int dmg) {
	c.damage(dmg);
	c.getHealthBar()->set(c.getHealth());
}


static void damage(Player& p, const int dmg) {
	p.damage(dmg);
}


AttackManager::~AttackManager() {
	for (auto p : list)
		delete p;
	// don't delete CreatureManager
}


void AttackManager::update(const Constants::float_type dt) {
	for (auto it = list.begin(); it != list.end();) {
		if ((**it).update(dt)) {
			delete *it;
			it = list.erase(it);
		}
		else
			++it;
	}
}


void AttackManager::draw(Canvas& can) {
	for (auto p : list)
		p->draw(can);
}


void AttackManager::add(Attack* a) {
	assert(a != nullptr);
	list.push_back(a);
}


bool AttackManager::procRect(const Attack* a, const SDL_Rect& rect, const int dmg) {
	assert(a != nullptr);
	switch (a->getTarget()) {
	case AttackTarget::NONE:
		return false;
	case AttackTarget::PLAYER:
		if (cm->intersectsPlayer(rect)) {
			damage(*cm->getPlayer(), dmg);
			return true;
		}
		else {
			return false;
		}
	case AttackTarget::CREATURES:
		return procRectCreatures(rect, dmg);
	}
	return false;
}


bool AttackManager::procCirc(const Attack* a, const Circle& circ, const int dmg) {
	assert(a != nullptr);
	switch (a->getTarget()) {
	case AttackTarget::NONE:
		return false;
	case AttackTarget::PLAYER:
		if (cm->intersectsPlayer(circ)) {
			damage(*cm->getPlayer(), dmg);
			return true;
		}
		else {
			return false;
		}
	case AttackTarget::CREATURES:
		return procCircCreatures(circ, dmg);
	}
	return false;
}


void AttackManager::setCreatureManager(CreatureManager* c) {
	cm = c;
}


bool AttackManager::procRectCreatures(const SDL_Rect& rect, const int dmg) {
	auto creatures = cm->getRect(rect);
	for (auto c : creatures)
		damage(*c, dmg);
	return !creatures.empty();
}


bool AttackManager::procCircCreatures(const Circle& circ, const int dmg) {
	bool ret = false;
	auto creatures = cm->getRect(Shape::bounds(circ));
	auto it = creatures.begin();
	while (it != creatures.end() && !ret) {
		if (Shape::intersects((**it).getBounds(), circ)) {
			damage(**it, dmg);
			ret = true;
		}
		++it;
	}
	while (it != creatures.end()) {
		if (Shape::intersects((**it).getBounds(), circ)) {
			damage(**it, dmg);
		}
		++it;
	}
	return ret;
}
