#include "gs_initial_screen.h"
#include "canvas.h"
#include "console.h"
#include "constants.h"
#include "exception.h"
#include "font.h"
#include "game_data.h"
#include "logger.h"
#include "resource_manager.h"
#include "state_manager.h"
#include "text_renderer.h"
#include "widget_layout.h"
#include "widget_progress_bar.h"
#include <boost/filesystem.hpp>
#include <string>
#define BOOST_FILESYSTEM_NO_DEPRECATED


namespace InitialScreenSettings {
	constexpr char dirCreateSuccess[] = "Created directory ";
	constexpr char dirCreateFailure[] = "Unable to create directory ";
	constexpr int barWidth = 150;
	constexpr int barHeight = 10;
	constexpr int barOutline = 1;
	constexpr int jobs = 6;
	constexpr Color colBg = COLOR_BLACK;
	constexpr Color colBar = COLOR_WHITE;
}


static void checkFolder(const std::string& path, bool writePerm) {
	namespace fs = boost::filesystem;
	using namespace InitialScreenSettings;
	fs::path p{path};
	fs::file_status status = fs::status(p);
	if (!fs::exists(status)) {
		// Try to create directory
		// If unable to create directory, report and continue
		try {
			if (fs::create_directory(p))
				Console::begin() << dirCreateSuccess << q(path) << std::endl;
			else
				Console::begin() << dirCreateFailure << q(path) << std::endl;
		}
		catch (fs::filesystem_error const& e) {
			Console::begin() << dirCreateFailure << q(path) << ". Details: " << e.what() << std::endl;
		}
	}
	status = fs::status(p);
	if (!fs::is_directory(status))
		Logger::instance().exit(FileError{path, FileError::Err::NOT_DIRECTORY});
	// check permissions
	const fs::perms perm = status.permissions();
	if (!(perm & fs::owner_read))
		Logger::instance().exit(FileError{path, "no read permissions"});
	if (writePerm & !(perm & fs::owner_write))
		Logger::instance().exit(FileError{path, "no write permissions"});
}


static void checkFolderExists(const std::string& path) {
	namespace fs = boost::filesystem;
	fs::path p{path};
	fs::file_status status = fs::status(p);
	if (!fs::exists(status))
		Logger::instance().exit(FileError{path, FileError::Err::MISSING});
	if (!fs::is_directory(status))
		Logger::instance().exit(FileError{path, FileError::Err::NOT_DIRECTORY});
}


InitialScreen::InitialScreen(std::shared_ptr<StateContext> sc) : GameState(StateType::INIT, sc), bar(new ProgressBar) {
	using namespace InitialScreenSettings;
	CommonCallback::setDefaults(getCallbacks());
	getCallbacks()->setKey(SDLK_ESCAPE, CommonCallback::popStateK);
	counter.setMaxTicks(jobs);
	bar->setPrefSize(IntPair{Constants::WSizeExpand, Constants::WSizeExpand});
	bar->setBackgroundColor(colBg);
	bar->setFillColor(colBar);
	bar->setMaxValue(jobs);
	bar->setOutlineSize(barOutline);
	VerticalLayout* layout = new VerticalLayout;
	layout->setMargins(0, 0, 0, 0);
	layout->add(bar);
	wArea.setSize(IntPair{barWidth, barHeight});
	wArea.setPosition(IntPair{	// center wArea
		(Constants::windowWidth - wArea.getSize().first) / 2,
		(Constants::windowHeight - wArea.getSize().second) / 2
	});
	wArea.setLayout(layout);
}


InitialScreen::~InitialScreen() {
	// do nothing
	// don't unload font or spritesheet since we want them always loaded
}


void InitialScreen::update() {
	if (counter.finished()) {
		GameData::instance().stateManager->switchTo(StateType::MENU);
		return;
	}
	switch (counter.getTicks()) {
	case 0:
		checkFolderExists(GameData::instance().dataPath);
		break;
	case 1:
		GameData::instance().resources->init();
		break;
	case 2:
		GameData::instance().resources->getSpriteSheet("default", true, true);
		break;
	case 3:
		GameData::instance().resources->getDefaultTR()->setFont(Font{Font::DEFAULT, Font::DEFAULT_SIZE}, true);
		GameData::instance().resources->getDefaultTR()->setRenderType(TextRenderType::BLENDED);
		break;
	case 4:
		GameData::instance().wData.init(GameData::instance().resources->getDefaultTR());
		break;
	case 5:
		checkFolder(GameData::instance().savePath, true);
		break;
	default:
		break;
	}
	counter.increment();
	bar->setValue(bar->getValue() + 1);
}


void InitialScreen::draw(Canvas& can) {
	can.setColor(InitialScreenSettings::colBg, SDL_ALPHA_OPAQUE);
	can.clearScreen();
	wArea.draw(can);
}


void InitialScreen::entered() {
}


void InitialScreen::leaving(const StateType, std::shared_ptr<StateContext>) {
}


void InitialScreen::obscuring(const StateType, std::shared_ptr<StateContext>) {
}


void InitialScreen::revealed(std::shared_ptr<StateContext>) {
}
