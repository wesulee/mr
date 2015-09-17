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


void CommonCallback::popState() {
	StateManager* const sm = GameData::instance().stateManager;
	if (!sm->eventWaiting())
		sm->pop();
}


// pop state on key press
void CommonCallback::popStateK(const KeyEvent& e) {
	StateManager* const sm = GameData::instance().stateManager;
	if (e.pressed && !sm->eventWaiting())
		sm->pop();
}


void CommonCallback::pushState(const StateType t) {
	StateManager* const sm = GameData::instance().stateManager;
	if (!sm->eventWaiting())
		sm->push(t);
}


void CommonCallback::pushStateK(const KeyEvent& e, const StateType t) {
	StateManager* const sm = GameData::instance().stateManager;
	if (e.pressed && !sm->eventWaiting())
		sm->push(t);
}


void CommonCallback::switchTo(const StateType t) {
	StateManager* const sm = GameData::instance().stateManager;
	if (!sm->eventWaiting())
		sm->switchTo(t);
}


void CommonCallback::setState(const StateType t) {
	GameData::instance().stateManager->set(t);
}


void CommonCallback::quit() {
	GameData::instance().stateManager->exit();
}
