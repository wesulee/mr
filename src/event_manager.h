#pragma once

#include "sdl_helper.h"
#include "input_handler.h"


class EventCallbackCollection;


class EventManager {
public:
	InputHandler* getInputHandler(void);
	void setCallbacks(EventCallbackCollection*);
	void clearMousePresses(void);
	void process(void);
private:
	SDL_Event e;
	InputHandler inputHandler;
};


inline
InputHandler* EventManager::getInputHandler() {
	return &inputHandler;
}


inline
void EventManager::setCallbacks(EventCallbackCollection* p) {
	inputHandler.setCallbacks(p);
}
