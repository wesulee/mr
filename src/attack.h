#pragma once

#include "constants.h"


class Canvas;
class Entity;


// who the attack will be affected by
enum class AttackTarget {NONE, PLAYER, CREATURES};


class Attack {
	Attack(const Attack&) = delete;
	void operator=(const Attack&) = delete;
public:
	Attack() = default;
	virtual ~Attack() {/* do nothing */}
	// return true to notify that attack can be deleted
	virtual bool update(const Constants::float_type) = 0;
	virtual void draw(Canvas&) = 0;
	void setSource(Entity* e) {source = e;}
	const Entity* getSource() const {return source;}
	void setTarget(const AttackTarget t) {target = t;}
	AttackTarget getTarget() const {return target;}
private:
	Entity* source = nullptr;
	AttackTarget target = AttackTarget::NONE;
};


/*
Every attack has a source and a target.
source is used to make sure that the source will not be affected by
  the attack. It will never be deleted or dereferenced.
*/
