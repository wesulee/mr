#include "input_handler.h"
#include "constants.h"
#include "widget.h"
#include <cassert>
#include <cstring>	// memcpy
#ifndef NDEBUG
#include <sstream>
#endif


const Uint8* InputHandler::keyState = nullptr;


InputHandler::InputHandler() : doProcess(&self_type::processNW) {
}


void InputHandler::init() {
	keyState = SDL_GetKeyboardState(nullptr);
}


void InputHandler::setTextInputWidget(Widget* w) {
	textInputWidget = w;
}


void InputHandler::startTextInput() {
	if (!textInputEnabled) {
		SDL_StartTextInput();
		textInputEnabled = true;
#if defined(DEBUG_IH_TEXT_INPUT) && DEBUG_IH_TEXT_INPUT
		DEBUG_BEGIN << DEBUG_IH_PREPEND << "TextInput ENABLED" << std::endl;
#endif
	}
}


void InputHandler::stopTextInput() {
	if (textInputEnabled) {
		SDL_StopTextInput();
		textInputEnabled = false;
#if defined(DEBUG_IH_TEXT_INPUT) && DEBUG_IH_TEXT_INPUT
		DEBUG_BEGIN << DEBUG_IH_PREPEND << "TextInput DISABLED" << std::endl;
#endif
	}
}


void InputHandler::setWidgetEvents(const bool b) {
	if (b)
		doProcess = &self_type::processW;
	else
		doProcess = &self_type::processNW;
}


void InputHandler::setWidgetEventCallback(WidgetEventCallback callback) {
	widgetCallback = callback;
}


// no WidgetEvent
void InputHandler::processNW(const SDL_Event& e) {
	switch (e.type) {
	case SDL_MOUSEMOTION:
		mouseX = e.button.x;
		mouseY= e.button.y;
		callbacks->mouseCallback(MouseEventType::MOVED);
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button != SDL_BUTTON_LEFT)
			break;
		mousePTime = e.button.timestamp;
		mouseIsPressed = true;
		callbacks->mouseCallback(MouseEventType::PRESSED);
		break;
	case SDL_MOUSEBUTTONUP:
		if (e.button.button != SDL_BUTTON_LEFT)
			break;
		mouseRTime = e.button.timestamp;
		mouseIsPressed = false;
		callbacks->mouseCallback(MouseEventType::RELEASED);
		break;
	case SDL_KEYDOWN:
		if (e.key.repeat != 0)
			break;
		keyEvent.key = e.key.keysym.sym;
		keyEvent.pressed = true;
		iter = callbacks->keyMap.find(e.key.keysym.sym);
		if (iter == callbacks->keyMap.end())
			callbacks->keyCallback(keyEvent);
		else
			iter->second(keyEvent);
		break;
	case SDL_KEYUP:
		keyEvent.key = e.key.keysym.sym;
		keyEvent.pressed = false;
		iter = callbacks->keyMap.find(e.key.keysym.sym);
		if (iter == callbacks->keyMap.end())
			callbacks->keyCallback(keyEvent);
		else
			iter->second(keyEvent);
		break;
	case SDL_TEXTINPUT:
	case SDL_TEXTEDITING:
		break;
	default:
		// should never happen since EventManager only passes these events
		assert(false);
		break;
	}
}


// generate WidgetEvent
// A mouse press and release always generates event, but if it is considered a click,
//   a click event is generated immediately following the release event.
// A click is when press and release time differ by <= Constants::WClickDur.
void InputHandler::processW(const SDL_Event& e) {
	switch (e.type) {
	case SDL_MOUSEMOTION:
		mouseX = e.button.x;
		mouseY = e.button.y;
		callbacks->mouseCallback(MouseEventType::MOVED);

		wEvent.type = WidgetEventType::MOUSE_MOVE;
		wEvent.move.x = mouseX;
		wEvent.move.y = mouseY;
		sendWidgetEvent();
		break;
	case SDL_MOUSEBUTTONDOWN:
		if (e.button.button != SDL_BUTTON_LEFT)
			break;

		mousePTime = e.button.timestamp;
		mouseIsPressed = true;
		callbacks->mouseCallback(MouseEventType::PRESSED);

		mousePX = e.button.x;
		mousePY = e.button.y;
		wEvent.type = WidgetEventType::MOUSE_PRESS;
		wEvent.press.x = mousePX;
		wEvent.press.y = mousePY;
		sendWidgetEvent();
		break;
	case SDL_MOUSEBUTTONUP:
		if (e.button.button != SDL_BUTTON_LEFT)
			break;
		mouseRTime = e.button.timestamp;
		mouseIsPressed = false;
		callbacks->mouseCallback(MouseEventType::RELEASED);

		wEvent.type = WidgetEventType::MOUSE_RELEASE;
		wEvent.release.x = e.button.x;
		wEvent.release.y = e.button.y;
		wEvent.release.time = e.button.timestamp - mousePTime;
		sendWidgetEvent();
		if ((wEvent.release.time <= Constants::WClickDur)
			&& (e.button.x == mousePX)
			&& (e.button.y == mousePY)
		) {
			wEvent.type = WidgetEventType::MOUSE_CLICK;
			wEvent.click.x = e.button.x;
			wEvent.click.y = e.button.y;
			sendWidgetEvent();
		}
		break;
	case SDL_KEYDOWN:
		if (e.key.repeat != 0)
			break;

		if (textInputEnabled && ((e.key.keysym.sym == SDLK_BACKSPACE) || (e.key.keysym.sym == SDLK_DELETE))) {
			wEvent.type = WidgetEventType::TEXT_DELETE;
			wEvent.textDelete.dirLeft = (e.key.keysym.sym == SDLK_BACKSPACE);
			wEvent.textDelete.modifier = (SDL_GetModState() & KMOD_CTRL);
			sendTextEvent();
			break;
		}

		keyEvent.key = e.key.keysym.sym;
		keyEvent.pressed = true;
		iter = callbacks->keyMap.find(e.key.keysym.sym);
		if (iter == callbacks->keyMap.end())
			callbacks->keyCallback(keyEvent);
		else
			iter->second(keyEvent);
		break;
	case SDL_KEYUP:
		if (textInputEnabled && ((e.key.keysym.sym == SDLK_BACKSPACE) || (e.key.keysym.sym == SDLK_DELETE)))
			break;

		keyEvent.key = e.key.keysym.sym;
		keyEvent.pressed = false;
		iter = callbacks->keyMap.find(e.key.keysym.sym);
		if (iter == callbacks->keyMap.end())
			callbacks->keyCallback(keyEvent);
		else
			iter->second(keyEvent);
		break;
	case SDL_TEXTINPUT:
		assert(textInputEnabled);
		wEvent.type = WidgetEventType::TEXT_INPUT;
		std::memcpy(wEvent.textInput.text, e.text.text, 32);
		sendTextEvent();
		break;
	case SDL_TEXTEDITING:
		assert(textInputEnabled);
		wEvent.type = WidgetEventType::TEXT_EDIT;
		sendTextEvent();
		break;
	default:
		// should never happen since EventManager only passes these events
		assert(false);
		break;
	}
}


void InputHandler::sendWidgetEvent() {
#ifndef NDEBUG
	std::stringstream ss;
	switch (wEvent.type) {
	case WidgetEventType::MOUSE_MOVE:
		ss << wEvent.move;
		break;
	case WidgetEventType::MOUSE_PRESS:
		ss << wEvent.press;
		break;
	case WidgetEventType::MOUSE_RELEASE:
		ss << wEvent.release;
		break;
	case WidgetEventType::MOUSE_CLICK:
		ss << wEvent.click;
		break;
	default:
		assert(false);	// wrong function or invalid event type
	}
	std::string str = ss.str();
	if (!str.empty())
		DEBUG_BEGIN << str << std::endl;
#endif // NDEBUG
	widgetCallback(wEvent);
}


void InputHandler::sendTextEvent() {
#ifndef NDEBUG
	std::stringstream ss;
	switch (wEvent.type) {
		case WidgetEventType::TEXT_INPUT:
		ss << wEvent.textInput;
		break;
	case WidgetEventType::TEXT_EDIT:
		ss << wEvent.textEdit;
		break;
	case WidgetEventType::TEXT_DELETE:
		ss << wEvent.textDelete;
		break;
	default:
		assert(false);	// wrong function or invalid event type
	}
	std::string str = ss.str();
	if (!str.empty())
		DEBUG_BEGIN << str << std::endl;
#endif // NDEBUG
	assert(textInputWidget != nullptr);
	textInputWidget->event(wEvent);
}
