#pragma once

#include "creature.h"
#include "animated_sprite.h"
#include "sdl_header.h"
#include "utility.h"


/*
Behavior: Always moving toward target when not in attack range.
When attacking: freeze position for some time, begin attack animation, then attack.
*/
class Creature1 : public Creature {
	enum class CreatureState {NONE, MOVING, ATTACKING, KNOCKBACK};
public:
	Creature1();
	~Creature1();
	void spawn(CreatureManager*, const int, const int) override;
	bool update(void) override;
	void draw(Canvas&) override;
	SDL_Rect getBounds(void) const override;
private:
	bool shouldAttack(void);
	void updateTargetPos(void);
	void updatePosition(void);
	bool facingLeft(void) const;
	void movingUpdate(void);

	SDL_Rect attackRect;
	UniformAnimatedSprite sprMovL;
	UniformAnimatedSprite sprMovR;
	Counter counter;
	AnimatedSprite* curSpr = nullptr;
	KillableGameEntity* target = nullptr;
	CreatureState state = CreatureState::NONE;
	int attackTicks = 0;
};
