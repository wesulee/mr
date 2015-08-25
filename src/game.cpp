#include "game.h"
#include "constants.h"
#include "game_data.h"
#include "game_state.h"
#include "input_handler.h"
#include "sdl_helper.h"
#include "settings.h"
#include <cstdint>
#include <iostream>


Game::Game(Settings*& settings) {
	// NOTE: Settings should be destroyed and set to nullptr
	GameData::instance().setDataPath(settings->dataPath);
	GameData::instance().setSavePath(settings->savePath);
	// update GameSettings
	GameData::instance().settings.set(
		GameSettings::Index::PAUSEFOCUSLOST,
		settings->getFlag(SettingsSettings::Index::PAUSEFOCUSLOST)
	);
	delete settings;
	settings = nullptr;
	stateManager.setEventManager(&eventManager);
	GameData::instance().resources = &resourceManager;
	GameData::instance().canvas = &canvas;
	GameData::instance().stateManager = &stateManager;
	GameData::instance().eventManager = &eventManager;
	GameData::instance().inputHandler = eventManager.getInputHandler();
}


bool Game::init(const uint32_t renderFlags = 0) {
	if (!SDL::init())
		return false;

	// init game components
	InputHandler::init();

	SDL::window = SDL_CreateWindow(
		"mr", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Constants::windowWidth, Constants::windowHeight, SDL_WINDOW_SHOWN
	);
	if (SDL::window == nullptr) {
		SDL::logError("Game::init CreateWindow");
		return false;
	}

	SDL::renderer = SDL_CreateRenderer(SDL::window, -1, renderFlags);
	if (SDL::renderer == nullptr) {
		SDL::logError("Game::init CreateRenderer");
		return false;
	}

	// set target texture support flag
	SDL_RendererInfo renInfo;
	if (SDL_GetRendererInfo(SDL::renderer, &renInfo) != 0) {
		SDL::logError("Game::init SDL_GetRendererInfo");
		return false;
	}
	#ifndef NDEBUG
	std::cout << "Using SDL renderer: " << renInfo.name << std::endl;
	#endif
	SDL::targetTextureSupport = renInfo.flags & SDL_RENDERER_TARGETTEXTURE;
	if (!SDL::targetTextureSupport)
		std::cout << "SDL_RENDERER_TARGETTEXTURE not available." << std::endl;

	if (SDL_SetRenderDrawBlendMode(SDL::renderer, SDL_BLENDMODE_BLEND) != 0) {
		SDL::logError("Game::init SDL_SetRenderDrawBlendMode");
		return false;
	}
	return true;
}


void Game::quit() {
	SDL_DestroyRenderer(SDL::renderer);
	SDL::renderer = nullptr;
	SDL_DestroyWindow(SDL::window);
	SDL::window = nullptr;
	SDL_Quit();
}


void Game::run() {
	uint32_t startTime, elapseTime, delayTime;
	// begin initial GameState
	stateManager.push(StateType::INIT);
	stateManager.processEvents();
	bool running = true;
	while (running) {
		startTime = SDL_GetTicks();
		GameData::instance().time = startTime;

		eventManager.process();
		stateManager.top()->update();
		draw();
		canvas.present();
		stateManager.processEvents();
		running = !stateManager.empty();
		
		elapseTime = SDL_GetTicks() - startTime;
		if (elapseTime > Constants::frameDuration)
			delayTime = Constants::frameDuration / 2;	// sleep anyway
		else
			delayTime = Constants::frameDuration - elapseTime;
		SDL_Delay(delayTime);
	}
}


void Game::draw() {
	stateManager.top()->draw(canvas);
	#if defined(DEBUG_MOUSE_POS) && DEBUG_MOUSE_POS
		auto oldColor = canvas.getColorState();
		const int x = GameData::instance().inputHandler->mX();
		const int y = GameData::instance().inputHandler->mY();
		canvas.setColor(DEBUG_MOUSE_POS_COLOR, SDL_ALPHA_OPAQUE);
		canvas.fillRect(x - DEBUG_MOUSE_POS_SZ, y, (DEBUG_MOUSE_POS_SZ * 2) + 1, 1);	// horiz
		canvas.fillRect(x, y - DEBUG_MOUSE_POS_SZ, 1, (DEBUG_MOUSE_POS_SZ * 2) + 1);	// vert
		canvas.setColorState(oldColor);
	#endif
}
