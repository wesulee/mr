#include "state_manager.h"
#include "event_manager.h"
#include "exception.h"
#include "game_state.h"
#include "state_context.h"
#ifndef NDEBUG
#include "constants.h"
#endif
// All GameStates
#include "main_menu.h"
#include "initial_screen.h"
#include "main_game.h"
#include "game_menu.h"
#include "load_menu.h"
#include "save_menu.h"
#include "dialog_state.h"


namespace StMHelper {

static void enter(GameState* gs) {
#if defined(DEBUG_StM_GS) && DEBUG_StM_GS
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "enter " << toString(gs->getType()) << std::endl;
#endif
	gs->entered();
}


static void leave(GameState* gs, const StateType st, std::shared_ptr<StateContext> sc) {
#if defined(DEBUG_StM_GS) && DEBUG_StM_GS
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "leave " << toString(gs->getType()) << " for " << toString(st) << std::endl;
#endif
	gs->leaving(st, sc);
}


static void obscure(GameState* gs, const StateType st, std::shared_ptr<StateContext> sc) {
#if defined(DEBUG_StM_GS) && DEBUG_StM_GS
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "obscure " << toString(gs->getType()) << " to " << toString(st) << std::endl;
#endif
	gs->obscuring(st, sc);
}


static void reveal(GameState* gs, std::shared_ptr<StateContext> sc) {
#if defined(DEBUG_StM_GS) && DEBUG_StM_GS
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "reveal " << toString(gs->getType()) << std::endl;
#endif
	gs->revealed(sc);
}


static void deleteGS(GameState* gs) {
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "delete " << toString(gs->getType()) << std::endl;
#endif
	delete gs;
}


template<class T>
T stackGet(std::stack<T>& stack, const std::size_t n) {
	if (n == 0)
		return stack.top();
	std::vector<T> vec{n};
	for (std::size_t i = 0; i < n; ++i) {
		vec.push_back(stack.top());
		stack.pop();
	}
	T ret = stack.top();
	for (std::size_t i = n; i-- > 0;)
		stack.push(vec[i]);
	return ret;
}


// returns item popped
static GameState* stackPop(std::stack<GameState*>& stack) {
#if defined(DEBUG_StM_STACK) && DEBUG_StM_STACK
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "stack pop " << toString(stack.top()->getType()) << std::endl;
#endif
	GameState* gs = stack.top();
	stack.pop();
	return gs;
}


static void stackPush(std::stack<GameState*>& stack, GameState* gs) {
#if defined(DEBUG_StM_STACK) && DEBUG_StM_STACK
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "stack push " << toString(gs->getType()) << std::endl;
#endif
	stack.push(gs);
}

}	// namespace StMHelper



StateManager::StateManager() : procEventsFunc(&self_type::doProcEvents) {
}


StateManager::~StateManager() {
	if (currentEvent != nullptr)
		delete currentEvent;
	if (futureEvent != nullptr)
		delete futureEvent;
	while (!states.empty())
		pop();
	// don't delete eventManager
}


void StateManager::push(const StateType st) {
	push(st, std::make_shared<StateContext>());
}


void StateManager::push(const StateType st, std::shared_ptr<StateContext> sc) {
	EventData* event = new EventData;
	event->eventType = EventType::PUSH;
	event->stateType = st;
	event->context = sc;
	addEvent(event);
}


void StateManager::pop() {
	pop(std::make_shared<StateContext>());
}


void StateManager::pop(std::shared_ptr<StateContext> sc) {
	EventData* event = new EventData;
	event->eventType = EventType::POP;
	event->context = sc;
	addEvent(event);
}


void StateManager::switchTo(const StateType st) {
	switchTo(st, std::make_shared<StateContext>());
}


void StateManager::switchTo(const StateType st, std::shared_ptr<StateContext> sc) {
	EventData* event = new EventData;
	event->eventType = EventType::SWITCH;
	event->stateType = st;
	event->context = sc;
	addEvent(event);
}


void StateManager::set(const StateType st) {
	set(st, std::make_shared<StateContext>());
}


void StateManager::set(const StateType st, std::shared_ptr<StateContext> sc) {
	EventData* event = new EventData;
	event->eventType = EventType::SET;
	event->stateType = st;
	event->context = sc;
	addEvent(event);
}


void StateManager::exit() {
	EventData* event = new EventData;
	event->eventType = EventType::EXIT;
	addEvent(event);
}


GameState* StateManager::get(const std::size_t n) {
	assert(n < states.size());
	return StMHelper::stackGet(states, n);
}


void StateManager::addEvent(EventData* event) {
	assert(event != nullptr);
	if (futureEvent == nullptr) {	// add event only if none waiting
		assert(futureEvent == nullptr);
		futureEvent = event;
		if (!futureEvent->context) {
			futureEvent->context = std::make_shared<StateContext>();
		}
	}
	else {
		// event ignored
		delete event;
	}
}


void StateManager::processPush(EventData* event) {
	using namespace StMHelper;
	StateType t = StateType::NONE;
	if (!states.empty()) {
		obscure(states.top(), event->stateType, event->context);
		t = states.top()->getType();
	}
	event->context->sourceType = t;
	GameState* gs = newGameState(event->stateType, event->context);
	stackPush(states, gs);
	stateChange();
	enter(gs);
}


void StateManager::processPop(EventData* event) {
	using namespace StMHelper;
	assert(!states.empty());
	GameState* const gs = states.top();
	stackPop(states);
	StateType t = StateType::NONE;
	if (!states.empty())
		t = states.top()->getType();
	leave(gs, t, event->context);	// notify gs that leaving to t and to update context
	event->context->sourceType = gs->getType();
	deleteGS(gs);
	if (!states.empty()) {
		stateChange();
		reveal(states.top(), event->context);
	}
}


void StateManager::processSwitch(EventData* event) {
	using namespace StMHelper;
	assert(!states.empty());
	leave(states.top(), event->stateType, event->context);
	event->context->sourceType = states.top()->getType();
	doPop();
	GameState* const gs = newGameState(event->stateType, event->context);
	stackPush(states, gs);
	stateChange();
	enter(gs);
}


void StateManager::processSet(EventData* event) {
	using namespace StMHelper;
	leave(states.top(), event->stateType, event->context);
	event->context->sourceType = states.top()->getType();
	clearStack();
	GameState* const gs = newGameState(event->stateType, event->context);
	stackPush(states, gs);
	stateChange();
	enter(gs);
	assert(states.size() == 1);
}


void StateManager::processExit(EventData*) {
	// Set new event processing method prior to clearing
	//   gamestates to ignore any future events
	procEventsFunc = &self_type::doProcEventsExit;
	clearStack();
}


void StateManager::doPop() {
	assert(!states.empty());
	GameState* gs = StMHelper::stackPop(states);
	StMHelper::deleteGS(gs);
}


void StateManager::clearStack() {
	while (!states.empty()) {
		StMHelper::leave(states.top(), StateType::NONE, std::shared_ptr<StateContext>{});
		doPop();
	}
}


// return true when want to recall this method
bool StateManager::doProcEvents() {
	if (futureEvent == nullptr)
		return false;	// no event
	assert(currentEvent == nullptr);
	currentEvent = futureEvent;
	futureEvent = nullptr;
	switch(currentEvent->eventType) {
	case EventType::PUSH:
		processPush(currentEvent);
		break;
	case EventType::POP:
		processPop(currentEvent);
		break;
	case EventType::SWITCH:
		processSwitch(currentEvent);
		break;
	case EventType::SET:
		processSet(currentEvent);
		break;
	case EventType::EXIT:
		processExit(currentEvent);
		break;
	}
	delete currentEvent;
	currentEvent = nullptr;
	return (futureEvent != nullptr);
}


bool StateManager::doProcEventsExit() {
	// do nothing
	return false;
}


//  Must be called when stack top has changed, but before (re)entering gamestate.
void StateManager::stateChange() {
	assert(eventManager != nullptr);
	eventManager->setCallbacks(states.top()->getCallbacks());
}


GameState* StateManager::newGameState(const StateType st, std::shared_ptr<StateContext> sc) {
	GameState* gs;
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
	DEBUG_BEGIN << DEBUG_StM_PREPEND << "new ";
#endif
	switch (st) {
	case StateType::INIT:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << toString(StateType::INIT) << std::endl;
#endif
		gs = new InitialScreen{sc};
		assert(gs->getType() == StateType::INIT);
		break;
	case StateType::MENU:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << toString(StateType::MENU) << std::endl;
#endif
		gs = new MainMenu{sc};
		assert(gs->getType() == StateType::MENU);
		break;
	case StateType::GAME:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << toString(StateType::GAME) << std::endl;
#endif
		gs = new MainGame{sc};
		assert(gs->getType() == StateType::GAME);
		break;
	case StateType::GAME_MENU:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << toString(StateType::GAME_MENU) << std::endl;
#endif
		gs = new GameMenu{sc};
		assert(gs->getType() == StateType::GAME_MENU);
		break;
	case StateType::LOAD_MENU:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << toString(StateType::LOAD_MENU) << std::endl;
#endif
		gs = new LoadMenu{sc};
		assert(gs->getType() == StateType::LOAD_MENU);
		break;
	case StateType::SAVE_MENU:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << toString(StateType::SAVE_MENU) << std::endl;
#endif
		gs = new SaveMenu{sc};
		assert(gs->getType() == StateType::SAVE_MENU);
		break;
	case StateType::DIALOG:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << toString(StateType::DIALOG) << std::endl;
#endif
		gs = new DialogState{sc};
		assert(gs->getType() == StateType::DIALOG);
		break;
	default:
#if defined(DEBUG_StM_NEW_DEL) && DEBUG_StM_NEW_DEL
		DEBUG_OS << "???" << std::endl;
#endif
		assert(false);	// did you forget to implement requested StateType?
		gs = nullptr;
		break;
	}
	if (gs == nullptr) {
		logAndExit(RuntimeError{"StateManager::newGameState", "unexpected nullptr"});
	}
	return gs;
}
