#include "load_menu.h"
#include "canvas.h"
#include "constants.h"
#include "dialog_data.h"
#include "event_manager.h"
#include "game_data.h"
#include "input_handler.h"
#include "menu_shared.h"
#include "parameters.h"
#include "sdl_header.h"
#include "state_context.h"
#include "state_manager.h"
#include "widget_data.h"
#include "widget_event.h"
#include "widget_layout.h"
#include "widget_text_item.h"
#include "widget_text_list_view.h"


namespace LoadMenuSettings {
	constexpr Color bgColor = COLOR_BLACK;
}


LoadMenu::LoadMenu(std::shared_ptr<StateContext> sc) : GameState(StateType::LOAD_MENU, sc) {
	using namespace MenuSettings;
	using std::placeholders::_1;
	getCallbacks()->setKey(SDLK_ESCAPE, CommonCallback::popState);
	getCallbacks()->setDefaultKey(DefaultCallback::key);
	getCallbacks()->setMouse(DefaultCallback::mouse);
	getCallbacks()->setEvent(DefaultCallback::event);
	// setup widgets
	VerticalLayout* layout = new VerticalLayout;
	layout->setMargins(0, 0, 0, 0);
	layout->setWidgetAlignment(WidgetAlignmentHoriz::CENTER, WidgetAlignmentVert::TOP);
	TextListView* view = new TextListView;
	view->setSelectedCallback(std::bind(&self_type::selectedCallback, this, _1));
	setup(*view);
	layout->add(view);
	wArea.setPosition(IntPair{viewPadding, viewPadding});
	wArea.setSize(IntPair{Constants::windowWidth - (viewPadding * 2), Constants::windowHeight - (viewPadding * 2)});
	wArea.setLayout(layout);
}


LoadMenu::~LoadMenu() {
	// don't delete selected
}


void LoadMenu::update() {
}


void LoadMenu::draw(Canvas& can) {
	can.setColor(LoadMenuSettings::bgColor, SDL_ALPHA_OPAQUE);
	can.clearScreen();
	wArea.draw(can);
}


void LoadMenu::entered() {
	enableWidgetEvents();
}


void LoadMenu::leaving(const StateType st, std::shared_ptr<StateContext> sc) {
	disableWidgetEvents();
	if ((st == StateType::GAME) && (selected != nullptr)) {
		sc->mIntStr[Parameters::LOAD_GAME] = selected->getText();
	}
}


void LoadMenu::obscuring(const StateType, std::shared_ptr<StateContext>) {
	disableWidgetEvents();
}


void LoadMenu::revealed(std::shared_ptr<StateContext> sc) {
	enableWidgetEvents();
	if (sc->sourceType == StateType::DIALOG) {
		const std::size_t i = GameData::instance().wData.dialogData->selected;
		GameData::instance().wData.dialogData = nullptr;
		if (i == 0) {	// yes to load
			GameData::instance().stateManager->set(StateType::GAME);
		}
		else {
			selected = nullptr;
		}
	}
}


void LoadMenu::event(WidgetEvent& e) {
	wArea.event(e);
	if ((selected != nullptr) && (e.type == WidgetEventType::MOUSE_RELEASE)) {
		GameData::instance().stateManager->push(StateType::DIALOG);
		GameData::instance().eventManager->clearMousePresses();
	}
}


void LoadMenu::selectedCallback(TextItem* item) {
	selected = item;
	if (item == nullptr)
		return;
	assert(!GameData::instance().wData.dialogData);
	std::shared_ptr<DialogData> data = std::make_shared<DialogData>();
	GameData::instance().wData.dialogData = data;
	data->title = "Load";
	data->message = "Are you sure you want to load " + item->getText() + '?';
	data->buttonText.reserve(2);
	data->buttonText.push_back("Yes");
	data->buttonText.push_back("No");
	// push state once mouse is released
}


void LoadMenu::enableWidgetEvents() {
	using std::placeholders::_1;
	InputHandler* const ih = GameData::instance().inputHandler;
	ih->setWidgetEvents(true);
	ih->setWidgetEventCallback(std::bind(&self_type::event, this, _1));
}


void LoadMenu::disableWidgetEvents() {
	GameData::instance().inputHandler->setWidgetEvents(false);
}
