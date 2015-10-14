#pragma once

#include "entity.h"
#include "health_bar_player.h"
#include "multistate_sprite.h"
#include "sdl_header.h"
#include "utility.h"
#include <cstddef>	// size_t


// use #define since static constexpr didn't want to work...
#define PLAYER_DIR_NONE 0
#define PLAYER_DIR_N 1
#define PLAYER_DIR_S (1 << 1)
#define PLAYER_DIR_W (1 << 2)
#define PLAYER_DIR_E (1 << 3)


class SaveData;
class Room;


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
	bool update(void) override;
	void draw(Canvas&) override;
	void setRoom(Room*);
	// Entity methods
	EntityResource* loadResource(void) override;
	void unloadResource(EntityResource*) override;
	SDL_Rect getBounds(void) const override;
	void updatePos(const float, const float) override;
	void setPos(const float, const float) override;
	void damage(const int) override;
	void setDirection(const int);
	void setSaveData(SaveData&) const;
	void getSaveData(const SaveData&);
private:
	void move(const float, const float);

	PlayerHealthBar healthBar;
	MultistateSprite ms;
	Room* room = nullptr;
	PlayerDirection direction = PlayerDirection::NONE;
	float speed;
	bool moving = false;
};
