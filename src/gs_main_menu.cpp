#include "gs_main_menu.h"
#include "canvas.h"
#include "font.h"
#include "game_data.h"
#include "input_handler.h"
#include "parameters.h"
#include "resource_manager.h"
#include "state_context.h"
#include "text_renderer.h"
#include "ui.h"
#include "widget_button.h"
#include "widget_layout.h"


namespace MainMenuSettings {
	constexpr int btnOutlineSize = 2;
	constexpr int btnTextPadH = 6;
	constexpr int btnTextPadV = 3;
	constexpr int paddingWidget = 10;
	constexpr int paddingBottom = 40;
	constexpr WidgetAlignmentHoriz alignH = WidgetAlignmentHoriz::CENTER;
	constexpr WidgetAlignmentVert alignV = WidgetAlignmentVert::BOTTOM;
	constexpr Color colBtnBgOut = COLOR_WHITE;
	constexpr Color colBtnBgOver = COLOR_MAGENTA;
	constexpr Color colBtnBgDown = COLOR_GREEN;
	constexpr Color colBtnOutline = COLOR_GREEN;
	constexpr Color colBtnText = COLOR_BLACK;
}


template<class Layout, class Style>
static void addButton(Layout layout, Style style, const char* str, ButtonCallback func) {
	TextButton* btn = new TextButton;
	btn->setStyle(style);
	btn->setText(str);
	btn->setCallback(func);
	layout->add(btn);
}


MainMenu::MainMenu(std::shared_ptr<StateContext> sc) : GameState(StateType::MENU, sc) {
	using namespace MainMenuSettings;
	getCallbacks()->setDefaultKey(DefaultCallback::key);
	getCallbacks()->setKey(SDLK_ESCAPE, CommonCallback::popStateK);
	getCallbacks()->setMouse(DefaultCallback::mouse);
	getCallbacks()->setEvent(DefaultCallback::event);

	TextRenderer tr;
	Font fontTitle{Font::DEFAULT, 40};
	tr.setFont(fontTitle);
	tr.setColor(COLOR_WHITE);
	SDL_Surface* surfTitle = tr.render("mr");
	titleBounds.x = 50;
	titleBounds.y = 50;
	titleBounds.w = surfTitle->w;
	titleBounds.h = surfTitle->h;
	texTitle = SDL::toTexture(surfTitle);
	// setup widgets
	VerticalLayout* layout = new VerticalLayout;
	layout->setWidgetAlignment(alignH, alignV);
	layout->setSpacing(paddingWidget);
	layout->setMargins(0, paddingBottom, 0, 0);
	std::shared_ptr<TextButton::Style> style = std::make_shared<TextButton::Style>();
	style->tr = GameData::instance().resources->getDefaultTR();
	style->outlineSz = btnOutlineSize;
	style->padTextH = btnTextPadH;
	style->padTextV = btnTextPadV;
	style->colText = colBtnText;
	style->colOutline = colBtnOutline;
	style->colBgOut = colBtnBgOut;
	style->colBgOver = colBtnBgOver;
	style->colBgDown = colBtnBgDown;
	addButton(layout, style, "New", std::bind(CommonCallback::setState, StateType::GAME));
	addButton(layout, style, "Load", std::bind(CommonCallback::pushState, StateType::LOAD_MENU));
	addButton(layout, style, "Quit", CommonCallback::quit);
	wArea.setPosition(IntPair{0, 0});
	wArea.setSize(IntPair{Constants::windowWidth, Constants::windowHeight});
	wArea.setLayout(layout);
}


MainMenu::~MainMenu() {
	SDL::free(texTitle);
}


void MainMenu::update(const Constants::float_type) {
}


void MainMenu::draw(Canvas& can) {
	can.setColor(COLOR_BLACK);
	can.clearScreen();
	can.draw(texTitle, &titleBounds);
	wArea.draw(can);
}


void MainMenu::entered() {
	enableWidgetEvents();
}


void MainMenu::leaving(const StateType st, std::shared_ptr<StateContext> sc) {
	disableWidgetEvents();
	switch (st) {
	case StateType::GAME:
		sc->mIntInt.emplace(Parameters::NEW_GAME, 0);	// value doesn't matter
		break;
	default:
		break;
	}
}


void MainMenu::obscuring(const StateType, std::shared_ptr<StateContext>) {
	disableWidgetEvents();
}


void MainMenu::revealed(std::shared_ptr<StateContext>) {
	enableWidgetEvents();
}


void MainMenu::enableWidgetEvents() {
	using std::placeholders::_1;
	InputHandler* const ih = GameData::instance().inputHandler;
	ih->setWidgetEvents(true);
	ih->setWidgetEventCallback(std::bind(&WidgetArea::event, &wArea, _1));
}


void MainMenu::disableWidgetEvents() {
	GameData::instance().inputHandler->setWidgetEvents(false);
}
