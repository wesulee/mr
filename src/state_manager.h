#pragma once

#include "state_type.h"
#include <cassert>
#include <cstddef>
#include <memory>
#include <stack>


class EventManager;
class GameState;
class StateContext;


/*
If a state requests multiple changes of state within a single update,
  only the first will be processed.
Event handling:
  Calls to push(), pop(), switchTo(), set(), and exit() generate an event, which is set to futureEvent
    if no event waiting to be processed.
  processEvents() checks if futureEvent is not null. If null, return immediately, else sets futureEvent as
    currentEvent, processes event, and deletes currentEvent. By the end of processevents(), futureEvent may
    not be null, and if so then repeat. This allows for multiple state changes with a single call.
Note: I would have used default argument for SC in push()/pop()/..., but it refused to work.
*/
class StateManager {
	StateManager(const StateManager&) = delete;
	void operator=(const StateManager&) = delete;
	typedef StateManager self_type;
	// PUSH: push a new state on stack
	// POP: pop stack (DOES NOT CHECK IF EMPTY)
	// SWITCH: pop current state and push new state (NO CHECKING)
	// SET: clear stack and push new state
	//   the state context passed to new state is the state at top of stack when called
	// EXIT: clear stack and ignore all subsequent requests
	enum class EventType {PUSH, POP, SWITCH, SET, EXIT};
	struct EventData {
		std::shared_ptr<StateContext> context;
		EventType eventType;
		StateType stateType;
	};
public:
	StateManager();
	~StateManager();
	void setEventManager(EventManager*);
	void processEvents(void);
	void push(const StateType);
	void push(const StateType, std::shared_ptr<StateContext>);
	void pop(void);
	void pop(std::shared_ptr<StateContext>);
	void switchTo(const StateType);
	void switchTo(const StateType, std::shared_ptr<StateContext>);
	void set(const StateType);
	void set(const StateType, std::shared_ptr<StateContext>);
	void exit(void);
	GameState* top(void);	// access current state
	GameState* get(const std::size_t);
	bool empty(void) const;	// is there an active GameState?
	// is there an event that has not been processed yet?
	bool stateChangeWaiting(void) const;
private:
	void addEvent(EventData*);
	void processPush(EventData*);
	void processPop(EventData*);
	void processSwitch(EventData*);
	void processSet(EventData*);
	void processExit(EventData*);
	void doPop(void);
	void clearStack(void);
	bool doProcEvents(void);		// actually process events
	bool doProcEventsExit(void);	// does nothing
	void stateChange(void);
	static GameState* newGameState(const StateType, std::shared_ptr<StateContext>);

	std::stack<GameState*> states;
	bool (self_type::*procEventsFunc)(void);
	EventData* currentEvent = nullptr;
	EventData* futureEvent = nullptr;
	EventManager* eventManager = nullptr;
};


inline
void StateManager::setEventManager(EventManager* p) {
	eventManager = p;
}


inline
void StateManager::processEvents() {
	// keep processing events to allow for multiple state changes within a single update
	while ((this->*procEventsFunc)())
		;
}


inline
GameState* StateManager::top() {
	assert(!states.empty());
	return states.top();
}


inline
bool StateManager::empty() const {
	return states.empty();
}


inline
bool StateManager::stateChangeWaiting() const {
	return (futureEvent != nullptr);
}
