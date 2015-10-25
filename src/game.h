#pragma once

#include "canvas.h"
#include "constants.h"
#include "event_manager.h"
#include "resource_manager.h"
#include "sdl_helper.h"
#include "state_manager.h"


class Settings;


class Game {
public:
	Game(Settings*&);
	~Game() = default;
	static bool init(const Settings&);
	static void quit(void);
	void run(void);
private:
	void update(const Constants::float_type, const Uint32);
	void draw(void);

	ResourceManager resourceManager;
	Canvas canvas;
	StateManager stateManager;
	EventManager eventManager;
	Constants::float_type dtMin;
	Constants::float_type dtMax;
};
