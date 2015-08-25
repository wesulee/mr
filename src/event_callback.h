#pragma once

#include "sdl_header.h"
#include "state_type.h"
#include <cstdint>
#include <functional>
#include <unordered_map>


class EventCallbackCollection;


enum class MouseEventType {PRESSED, RELEASED, MOVED};


struct KeyEvent {
	SDL_Keycode key;
	bool pressed;
};


typedef std::function<void(const KeyEvent&)> KeyCallback;
typedef std::function<void(const MouseEventType)> MouseCallback;
typedef std::function<void(const SDL_Event&)> EventCallback;


namespace DefaultCallback {
	inline void key(const KeyEvent& e) {(void)e;}
	inline void mouse(const MouseEventType e) {(void)e;}
	inline void event(const SDL_Event& e) {(void)e;}
}


namespace CommonCallback {
	void setDefaults(EventCallbackCollection* const);

	void popState(const KeyEvent&);
	void popState2(void);
	void pushState(const KeyEvent&, const StateType);
	void switchTo(const StateType);
	void quit(void);
}


// Used by EventManager and InputHandler
struct EventCallbackCollection {
	typedef std::unordered_map<SDL_Keycode, KeyCallback> KeyCallbackMap;

	void setKey(const SDL_Keycode, KeyCallback);
	void setDefaultKey(KeyCallback);
	void setMouse(MouseCallback);
	void setEvent(EventCallback);

	KeyCallbackMap keyMap;
	KeyCallback keyCallback;	// default
	MouseCallback mouseCallback;
	EventCallback eventCallback;
};


inline
void EventCallbackCollection::setKey(const SDL_Keycode k, KeyCallback f) {
	keyMap[k] = f;
}


inline
void EventCallbackCollection::setDefaultKey(KeyCallback f) {
	keyCallback = f;
}


inline
void EventCallbackCollection::setMouse(MouseCallback f) {
	mouseCallback = f;
}


inline
void EventCallbackCollection::setEvent(EventCallback f) {
	eventCallback = f;
}
