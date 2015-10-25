#include "game.h"
#include "console.h"
#include "game_data.h"
#include "game_state.h"
#include "input_handler.h"
#include "settings.h"
#include "utility.h"	// q
#include <algorithm>	// max
#include <cassert>
#include <cmath>	// ceil
#include <cstring>	// strcmp
#include <iostream>


// NOTE: Settings should be destroyed and set to nullptr before returning
Game::Game(Settings*& settings) {
	dtMin = static_cast<Constants::float_type>(1.0 / settings->maxFPS);
	dtMax = static_cast<Constants::float_type>(1.0 / settings->minFPS);
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


// Variable time-step (dt, with fixed min/max).
// Game progresses at constant time, unless FPS drops below a threshold.
void Game::run() {
	// begin initial GameState
	stateManager.push(StateType::INIT);
	stateManager.processEvents();

	Uint32 prevTime;
	Uint32 curTime;
	Uint32 elapseTime;
	Constants::float_type elapse;
	int frameSkip;
	bool running = true;
	prevTime = SDL_GetTicks();
	while (running) {
		frameSkip = 0;
		curTime = SDL_GetTicks();
		elapseTime = (curTime - prevTime);
		elapse = static_cast<Constants::float_type>(elapseTime) / 1000;
		while (elapse < dtMin) {
			// running too fast
			// while loop rather than just an if because for some reason one
			//   call to SDL_Delay sometimes isn't enough
			SDL_Delay(std::max(
				static_cast<Uint32>(1),
				static_cast<Uint32>(std::ceil((dtMin - elapse) * 1000))
			));
			curTime = SDL_GetTicks();
			elapseTime = (curTime - prevTime);
			elapse = static_cast<Constants::float_type>(elapseTime) / 1000;
		}
		prevTime = curTime;
		if (elapse > dtMax) {
			// running too slow
			do {
				update(dtMax, SDL_GetTicks());
				elapse -= dtMax;
				++frameSkip;
			}
			while ((elapse > dtMax) && (frameSkip < Constants::maxFrameSkip));
		}
		else {
			assert(elapse >= dtMin);
			update(elapse, curTime);
		}
		draw();
		canvas.present();
		stateManager.processEvents();
		running = !stateManager.empty();
	}
}


void Game::update(const Constants::float_type dt, const Uint32 cTime) {
	eventManager.process();
	GameData::instance().time = cTime;
	stateManager.top()->update(dt);
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
