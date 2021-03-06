#pragma once

#include "entity.h"
#include "health_bar_player.h"
#include "multistate_sprite.h"
#include "spell_type.h"
#include <cstddef>	// size_t


// use #define since static constexpr didn't want to work...
#define PLAYER_DIR_NONE 0
#define PLAYER_DIR_N 1
#define PLAYER_DIR_S (1 << 1)
#define PLAYER_DIR_W (1 << 2)
#define PLAYER_DIR_E (1 << 3)


class SaveData;
class Spell;


enum class PlayerDirection {NONE, N, E, S, W, NE, SE, SW, NW};
enum class PlayerState {
	//! todo: implement...
	NONE,
	L, R,		// standing left or right
	W_L, W_R,	// walking
	C_L, C_R,	// casting (standing)
	CW_L, CW_R,	// casting and walking
	K_L, K_R,	// knockback when previously L, R, W_L, W_R
	CK_L, CK_R	// knockback when previously C_L, C_R, CW_L, CW_R
};


/*
Player direction is an integer such that if DIR_N bit is set, then direction is north, etc.
Directions can cancel, so if N and S are set, direction is NONE.
*/
class Player : public KillableGameEntity {
	enum : std::size_t {SPR_STATE_LEFT=0, SPR_STATE_RIGHT};
public:
	Player();
	~Player();
	bool update(const Constants::float_type) override;
	void draw(Canvas&) override;
	void mousePress(void);
	void mouseRelease(void);
	HealthBar* getHealthBar(void);
	// Entity methods
	EntityResource* loadResource(void) override;
	void unloadResource(EntityResource*) override;
	SDL_Rect getBounds(void) const override;
	void damage(const int) override;
	void setDirection(const int);
	void setSaveData(SaveData&) const;
	void getSaveData(const SaveData&);
private:
	void move(const Constants::float_type, const Constants::float_type);
	void updateSpellPos(void);

	PlayerHealthBar healthBar;
	MultistateSprite ms;
	Spell* spell = nullptr;
	SpellType spellType = SpellType::BASIC;
	PlayerDirection direction = PlayerDirection::NONE;
	Constants::float_type speed;	// max dist traveled in one direction per second
	bool moving = false;
};


inline
HealthBar* Player::getHealthBar() {
	return &healthBar;
}
