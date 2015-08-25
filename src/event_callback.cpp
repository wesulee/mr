#include "event_callback.h"
#include "game_data.h"
#include "state_manager.h"
#include <cassert>


// note: does not modify keyMap
void CommonCallback::setDefaults(EventCallbackCollection* const p) {
	p->setDefaultKey(DefaultCallback::key);
	p->setMouse(DefaultCallback::mouse);
	p->setEvent(DefaultCallback::event);
}


// pop state on key release
void CommonCallback::popState(const KeyEvent& e) {
	StateManager* const sm = GameData::instance().stateManager;
	if (e.pressed && !sm->stateChangeWaiting()) {
		sm->pop();
	}
}


void CommonCallback::popState2() {
	StateManager* const sm = GameData::instance().stateManager;
	if (!sm->stateChangeWaiting())
		sm->pop();
}


void CommonCallback::pushState(const KeyEvent& e, const StateType t) {
	StateManager* const sm = GameData::instance().stateManager;
	if (e.pressed && !sm->stateChangeWaiting()) {
		sm->push(t);
	}
}


void CommonCallback::switchTo(const StateType t) {
	StateManager* const sm = GameData::instance().stateManager;
	if (!sm->stateChangeWaiting())
		sm->switchTo(t);
}


void CommonCallback::quit() {
	GameData::instance().stateManager->exit();
}
