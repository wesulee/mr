#pragma once

#include "event_callback.h"
#include "sdl_header.h"
#include "widget_event.h"
#include <cstdint>
#include <functional>


class Widget;


typedef std::function<void(WidgetEvent&)> WidgetEventCallback;


class InputHandler {
	typedef InputHandler self_type;
public:
	InputHandler();
	~InputHandler() {}
	static void init(void);

	bool isPressed(const SDL_Keycode);
	// mouse functions
	int mX(void) const;
	int mY(void) const;
	bool mousePressed(void) const;
	uint32_t mouseDur(void) const;
	// text input
	bool isTextInputEnabled(void) const;
	void setTextInputWidget(Widget*);
	void startTextInput(void);
	void stopTextInput(void);
	// widget
	void setWidgetEvents(const bool);
	void setWidgetEventCallback(WidgetEventCallback);
	// should only be called by EventManager
	void process(const SDL_Event&);
	void setCallbacks(EventCallbackCollection*);
	EventCallbackCollection* getCallbacks(void);
private:
	void processNW(const SDL_Event&);
	void processW(const SDL_Event&);
	void sendWidgetEvent(void);
	void sendTextEvent(void);

	EventCallbackCollection::KeyCallbackMap::iterator iter;
	EventCallback textCallback;
	WidgetEventCallback widgetCallback;
	WidgetEvent wEvent;
	KeyEvent keyEvent;
	void (self_type::*doProcess)(const SDL_Event&);
	static const Uint8* keyState;	// note: ignores shift
	EventCallbackCollection* callbacks;
	Widget* textInputWidget = nullptr;
	int mouseX = 0;
	int mouseY = 0;
	int mousePX = 0;	// press coordinates
	int mousePY = 0;
	uint32_t mousePTime = 0;	// pressed time, ms
	uint32_t mouseRTime = 0;	// released time, ms
	bool mouseIsPressed = false;
	bool textInputEnabled = false;
	bool widgetEventsEnabled = false;
};


inline
bool InputHandler::isPressed(const SDL_Keycode k) {
	// keyState[k] == 0 not pressed, 1 pressed
	return (keyState[SDL_GetScancodeFromKey(k)] != 0);
}


inline
int InputHandler::mX() const {
	return mouseX;
}


inline
int InputHandler::mY() const {
	return mouseY;
}


inline
bool InputHandler::mousePressed() const {
	return mouseIsPressed;
}


// after mouse release event, call this to determine how long (in ms) mouse was pressed
inline
uint32_t InputHandler::mouseDur() const {
	return (mouseRTime - mousePTime);
}


inline
bool InputHandler::isTextInputEnabled() const {
	return textInputEnabled;
}


inline
void InputHandler::setCallbacks(EventCallbackCollection* p) {
	callbacks = p;
}


inline
EventCallbackCollection* InputHandler::getCallbacks() {
	return callbacks;
}


inline
void InputHandler::process(const SDL_Event& e) {
	(this->*doProcess)(e);
}
