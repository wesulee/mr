#pragma once

#include "canvas.h"
#include "event_manager.h"
#include "resource_manager.h"
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
	void draw(void);

	ResourceManager resourceManager;
	Canvas canvas;
	StateManager stateManager;
	EventManager eventManager;
};
