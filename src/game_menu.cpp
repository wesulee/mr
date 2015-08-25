#include "game_menu.h"
#include "canvas.h"
#include "constants.h"
#include "font.h"
#include "game_data.h"
#include "image.h"
#include "input_handler.h"
#include "parameters.h"
#include "resource_manager.h"
#include "sdl_header.h"
#include "state_context.h"
#include "state_manager.h"
#include "text_renderer.h"
#include "ui.h"
#include "widget_button.h"
#include "widget_layout.h"
#include <cassert>


namespace GameMenuSettings {
	constexpr int paddingWidget = 4;
	constexpr WidgetAlignmentHoriz alignH = WidgetAlignmentHoriz::CENTER;
	constexpr WidgetAlignmentVert alignV = WidgetAlignmentVert::BOTTOM;
	constexpr Color colBtnBg = COLOR_BLACK;
	constexpr Color colBtnTextOut = COLOR_WHITE;
	constexpr Color colBtnTextOver = COLOR_YELLOW;
	constexpr Color colBtnTextDown = COLOR_YELLOW;
	constexpr Uint8 alphaBtnOut = getAlpha<60>();
	constexpr Uint8 alphaBtnOver = getAlpha<70>();
	constexpr Uint8 alphaBtnDown = getAlpha<80>();
}


template<class T>
static void addButton(VerticalLayout* layout, T style, const char* str, ButtonCallback func) {
	TextButton2* btn = new TextButton2;
	btn->setStyle(style);
	btn->setText(str);
	btn->setCallback(func);
	layout->add(btn);
}


GameMenu::GameMenu(std::shared_ptr<StateContext> sc) : GameState(StateType::GAME_MENU, sc) {
	using namespace GameMenuSettings;
	assert(sc->mIntInt.count(Parameters::MAIN_GAME_IS_RUNNING));
	getCallbacks()->setKey(SDLK_ESCAPE, CommonCallback::popState);
	getCallbacks()->setKey(SDLK_p, CommonCallback::popState);
	getCallbacks()->setDefaultKey(DefaultCallback::key);
	getCallbacks()->setMouse(DefaultCallback::mouse);
	getCallbacks()->setEvent(DefaultCallback::event);
	// check if background image given
	auto backgroundFind = sc->images.find("background");
	assert(backgroundFind != sc->images.end());
	background = backgroundFind->second;
	// setup widgets
	VerticalLayout* layout = new VerticalLayout;
	layout->setMargins(0, paddingWidget, 0, 0);
	layout->setWidgetAlignment(alignH, alignV);
	layout->setSpacing(paddingWidget);
	std::shared_ptr<TextButton2::Style> style = std::make_shared<TextButton2::Style>();
	TextRenderer tr;
	tr.setFont(Font{Font::DEFAULT, 20});
	style->tr = &tr;
	style->outlineSz = 0;
	style->colTextOut = colBtnTextOut;
	style->colTextOver = colBtnTextOver;
	style->colTextDown = colBtnTextDown;
	style->alphaOut = alphaBtnOut;
	style->alphaOver = alphaBtnOver;
	style->alphaDown = alphaBtnDown;
	addButton(layout, style, "Resume", CommonCallback::popState2);
	if (!sc->mIntInt.at(Parameters::MAIN_GAME_IS_RUNNING))
		addButton(layout, style, "Save", std::bind(&self_type::saveCallback, this));
	addButton(layout, style, "Load", std::bind(CommonCallback::switchTo, StateType::LOAD_MENU));
	addButton(layout, style, "Quit", CommonCallback::quit);
	style->tr = nullptr;	// button text has been rendered and is no longer needed
	wArea.setPosition(IntPair{0, 0});
	wArea.setSize(IntPair{Constants::windowWidth, Constants::windowHeight});
	wArea.setLayout(layout);
}


GameMenu::~GameMenu() {
}


void GameMenu::update() {
}


void GameMenu::draw(Canvas& can) {
	can.draw(*background);
	can.setColor(COLOR_BLACK, getAlpha<20>());
	can.fillRect(0, 0, Constants::windowWidth, Constants::windowHeight);
	wArea.draw(can);
}


void GameMenu::entered() {
	enableWidgetEvents();
}


void GameMenu::leaving(const StateType, std::shared_ptr<StateContext>) {
	disableWidgetEvents();
}


void GameMenu::obscuring(const StateType, std::shared_ptr<StateContext>) {
	disableWidgetEvents();
}


void GameMenu::revealed(std::shared_ptr<StateContext>) {
	enableWidgetEvents();
}


void GameMenu::saveCallback() {
	GameData::instance().stateManager->switchTo(StateType::SAVE_MENU);
}


void GameMenu::enableWidgetEvents() {
	using std::placeholders::_1;
	InputHandler* const ih = GameData::instance().inputHandler;
	ih->setWidgetEvents(true);
	ih->setWidgetEventCallback(std::bind(&WidgetArea::event, &wArea, _1));
}


void GameMenu::disableWidgetEvents() {
	GameData::instance().inputHandler->setWidgetEvents(false);
}
