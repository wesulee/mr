#pragma once

#include "constants.h"	// float_type
#include "event_callback.h"
#include "state_type.h"
#include <memory>


class Canvas;
class StateContext;


// Base class for all GameStates
// May assume that all given StateContexts are not null
//   Exception: leaving(), when StateType is NONE, context may be nullptr
class GameState {
public:
	GameState(const StateType, std::shared_ptr<StateContext>);
	virtual ~GameState() {}
	// time delta in seconds
	virtual void update(const Constants::float_type);
	virtual void draw(Canvas&) = 0;
	virtual StateType getType() const final;
	// called once, after created and becomes primary state
	virtual void entered(void) = 0;
	// called before deleted, leaving to specified ST
	virtual void leaving(const StateType, std::shared_ptr<StateContext>) = 0;
	// called before another GS pushed, obscuring to specified ST
	virtual void obscuring(const StateType, std::shared_ptr<StateContext>) = 0;
	// called after returning from some state that provided context
	virtual void revealed(std::shared_ptr<StateContext>) = 0;
	// various callbacks that are called on certain events
	EventCallbackCollection* getCallbacks(void);
private:
	EventCallbackCollection callbacks;
	StateType state = StateType::NONE;
};


inline
GameState::GameState(const StateType t, std::shared_ptr<StateContext> sc) : state(t) {
	(void)sc;
}


inline
void GameState::update(const Constants::float_type) {
}


inline
StateType GameState::getType() const {
	return state;
}


inline
EventCallbackCollection* GameState::getCallbacks() {
	return &callbacks;
}
