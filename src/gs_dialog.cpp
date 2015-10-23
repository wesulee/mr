#include "gs_dialog.h"
#include "canvas.h"
#include "exception.h"
#include "game_data.h"
#include "input_handler.h"
#include "logger.h"
#include "state_manager.h"
#include "widget_data.h"
#include "widget_dialog.h"
#include "widget_layout.h"
#include <cassert>


DialogState::DialogState(std::shared_ptr<StateContext> sc) : GameState(StateType::DIALOG, sc) {
	getCallbacks()->setDefaultKey(DefaultCallback::key);
	getCallbacks()->setMouse(DefaultCallback::mouse);
	getCallbacks()->setEvent(DefaultCallback::event);

	if (!GameData::instance().wData.dialogData) {
		Logger::instance().exit(RuntimeError{"DialogState ctor", "null data"});
		return;
	}
	dialog = new Dialog{GameData::instance().wData.dialogData};
	VerticalLayout* layout = new VerticalLayout;
	layout->setMargins(0, 0, 0, 0);
	layout->setWidgetAlignment(WidgetAlignmentHoriz::CENTER, WidgetAlignmentVert::CENTER);
	layout->add(dialog);
	wArea.setPosition(IntPair{0, 0});
	wArea.setSize(IntPair{Constants::windowWidth, Constants::windowHeight});
	wArea.setLayout(layout);
}


DialogState::~DialogState() {
	// do not delete dialog
}


void DialogState::update(const Constants::float_type) {
	if (dialog->done())
		GameData::instance().stateManager->pop();
}


void DialogState::draw(Canvas& can) {
	wArea.draw(can);
}


void DialogState::entered() {
	enableWidgetEvents();
}


void DialogState::leaving(const StateType, std::shared_ptr<StateContext>) {
	disableWidgetEvents();
}


void DialogState::obscuring(const StateType, std::shared_ptr<StateContext>) {
	assert(false);	// revealed/obscuring not allowed
}


void DialogState::revealed(std::shared_ptr<StateContext>) {
	assert(false);	// revealed/obscuring not allowed
}


void DialogState::enableWidgetEvents() {
	using std::placeholders::_1;
	InputHandler* const ih = GameData::instance().inputHandler;
	ih->setWidgetEvents(true);
	ih->setWidgetEventCallback(std::bind(&WidgetArea::event, &wArea, _1));
}


void DialogState::disableWidgetEvents() {
	GameData::instance().inputHandler->setWidgetEvents(false);
}
