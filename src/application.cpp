#include "application.h"
#include "console.h"
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
	Console::startTimer();
	doRun();
	Console::flush();
	return GameData::instance().exitCode;
}


void Application::doRun() {
	if (settings->exitFlag)
		return;
	if (!Game::init(*settings))
		return;

	{
		Game game{settings};
		assert(settings == nullptr);
		game.run();
	}
	Game::quit();
}
