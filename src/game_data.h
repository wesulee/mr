#pragma once

#include "game_settings.h"
#include "widget_data.h"
#include <cstdint>
#include <memory>
#include <random>
#include <string>


class Canvas;
class EventManager;
class InputHandler;
class MainGameObjects;
class ResourceManager;
class StateManager;


// Global game constants and data
// Paths are absolute
class GameData {
	GameData(const GameData&);
	void operator=(GameData const&);
public:
	static GameData& instance(void);
	~GameData() {/* do nothing */}
	void setDataPath(const std::string&);
	void setSavePath(const std::string&);

	WidgetData wData;
	std::string dataPath;
	std::string savePath;
	GameSettings settings;
	Canvas* canvas = nullptr;
	EventManager* eventManager = nullptr;
	InputHandler* inputHandler = nullptr;
	MainGameObjects* mgo = nullptr;
	ResourceManager* resources = nullptr;
	StateManager* stateManager = nullptr;
	std::default_random_engine randGen;
	uint32_t time = 0;	// ms
	int exitCode;
private:
	GameData();
	static std::random_device::result_type seed(void);
};


inline
GameData& GameData::instance() {
	static GameData gd;
	return gd;
}
