#pragma once

#include "animated_sprite.h"
#include "creature.h"
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
	bool update(const Constants::float_type) override;
	void draw(Canvas&) override;
	SDL_Rect getBounds(void) const override;
private:
	bool shouldAttack(void);
	void updateTargetPos(void);
	void updatePosition(const Constants::float_type);
	bool facingLeft(void) const;
	void movingUpdate(const Constants::float_type);

	SDL_Rect attackRect;
	UniformAnimatedSprite sprMovL;
	UniformAnimatedSprite sprMovR;
	Vector2D<> vel;
	AnimatedSprite* curSpr = nullptr;
	KillableGameEntity* target = nullptr;
	CreatureState state = CreatureState::NONE;
	int attackTicks = 0;
	Constants::float_type time = 0;
};
