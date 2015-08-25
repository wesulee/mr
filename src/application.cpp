#include "application.h"
#include "game.h"
#include "game_data.h"	// exitCode
#include "settings.h"
#include <cassert>


Application::Application(int argc, char** argv) : settings(new Settings{argc, argv}) {
}


Application::~Application() {
	if (settings != nullptr)
		delete settings;
}


int Application::run() {
	doRun();
	return GameData::instance().exitCode;
}


void Application::doRun() {
	if (settings->exitFlag)
		return;
	if (!Game::init(settings->rendererFlags()))
		return;

	{
		Game game{settings};
		assert(settings == nullptr);
		game.run();
	}
	Game::quit();
}
