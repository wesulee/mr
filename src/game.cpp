#include "game.h"
#include "console.h"
#include "constants.h"
#include "game_data.h"
#include "game_state.h"
#include "input_handler.h"
#include "sdl_helper.h"
#include "settings.h"
#include "utility.h"	// q
#include <cstdint>
#include <cstring>
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


bool Game::init(const Settings& settings) {
	if (!SDL::init())
		return false;

	// init game components
	InputHandler::init();

	SDL::window = SDL::createWindow(
		"mr", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
		Constants::windowWidth, Constants::windowHeight, SDL_WINDOW_SHOWN
	);

	// Create renderer
	SDL_RendererInfo renInfo;
	auto rendererPref = settings.getRendererPref();
	int rendererIndex = -1;		// default value
	SDL_SetHintWithPriority(	// set vsync preference
		SDL_HINT_RENDER_VSYNC,
		rendererPref.second ? SDLHintValue::TRUE : SDLHintValue::FALSE,
		SDL_HINT_OVERRIDE
	);
	if (!rendererPref.first.empty()) {
		// preference is set, search for renderer
		const int availDrivers = SDL::getNumRenderDrivers();
		for (int i = 0; i < availDrivers; ++i) {
			SDL::getRenderDriverInfo(i, &renInfo);
			if (std::strcmp(rendererPref.first.c_str(), renInfo.name) == 0) {
				rendererIndex = i;
				break;
			}
		}
		if (rendererIndex == -1) {
			Console::begin() << "The renderer " << q(rendererPref.first)
			                 << " is not available. Will use default renderer instead." << std::endl;
		}
	}
	SDL::renderer = SDL::createRenderer(SDL::window, rendererIndex, 0);
	SDL::getRendererInfo(SDL::renderer, &renInfo);
	std::string rendererFlags = SDL::rendererFlagsToString(renInfo.flags);
	if (rendererFlags.empty())
		rendererFlags = "none";
	Console::begin() << "renderer: " << q(renInfo.name) << " flags: " << rendererFlags << std::endl;
	// set target texture support flag
	SDL::targetTextureSupport = ((renInfo.flags & SDL_RENDERER_TARGETTEXTURE) != 0);
	if (!SDL::targetTextureSupport)
		Console::begin() << "Warning: TARGETTEXTURE not available." << std::endl;
	SDL::setRenderDrawBlendMode(SDL::renderer, SDL_BLENDMODE_BLEND);
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
	const Constants::float_type dt = (1.0 / 60);
	Constants::float_type elapse;
	bool running = true;
	while (running) {
		startTime = SDL_GetTicks();
		GameData::instance().time = startTime;

		eventManager.process();
		stateManager.top()->update(dt);
		draw();
		canvas.present();
		stateManager.processEvents();
		running = !stateManager.empty();
		
		elapseTime = SDL_GetTicks() - startTime;
		elapse = static_cast<Constants::float_type>(elapseTime) / 1000;
		if (elapse > dt)
			delayTime = static_cast<Uint32>(dt / 2 * 1000);	// sleep anyway
		else
			delayTime = static_cast<Uint32>((dt - elapse) * 1000);
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
#endif // DEBUG_MOUSE_POS
}
