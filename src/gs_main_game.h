#pragma once

#include "game_state.h"
#include "sdl_helper.h"
// Game components
#include "game_interface.h"
#include "creature_manager.h"
#include "player.h"
#include "attack_manager.h"
#include "room.h"
#include "spell_manager.h"
#include "vfx_manager.h"
#include "map.h"
#include "main_game_objects.h"


class MainGame : public GameState {
	typedef MainGame self_type;
public:
	MainGame(std::shared_ptr<StateContext>);
	~MainGame();
	void update(const Constants::float_type) override;
	void draw(Canvas&) override;
	void entered(void) override;
	void leaving(const StateType, std::shared_ptr<StateContext>) override;
	void obscuring(const StateType, std::shared_ptr<StateContext>) override;
	void revealed(std::shared_ptr<StateContext>) override;
private:
	void updateRunning(const Constants::float_type);
	void updateCleared(const Constants::float_type);
	void drawRunning(Canvas&);
	void drawCleared(Canvas&);
	void obscureToMenu(std::shared_ptr<StateContext>);
	void mouseCallbackRunning(const MouseEventType);
	void mouseCallbackCleared(const MouseEventType);
	void playerDirCallback(const KeyEvent&, const int);
	void eventCallback(const SDL_Event&);
	void refreshPlayerDir(void);
	void setRunningFunc(void);
	void setClearedFunc(void);
	void load(const std::string&);
	void save(const std::string&);

	Player player;
	SpellManager sm;
	Room room;
	CreatureManager cm;
	AttackManager am;
	VFXManager vfxm;
	Map map;
	GameInterface gi;
	MainGameObjects objects;
	SDL_Keycode playerDirKeys[4];	// N, E, S, W
	int playerDirection = PLAYER_DIR_NONE;
	void (self_type::*updateFunc)(const Constants::float_type);
	void (self_type::*drawFunc)(Canvas&);
};
