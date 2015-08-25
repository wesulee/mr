#include "event_manager.h"
#include "event_callback.h"
#include "game_data.h"
#include "state_manager.h"

#ifndef NDEBUG
#include "constants.h"
#include <iomanip>
#include <iostream>
#include <sstream>
#include <string>

static std::string EventToString(const SDL_Event&);
#endif


void EventManager::clearMousePresses() {
	SDL_FlushEvents(SDL_MOUSEBUTTONDOWN, SDL_MOUSEBUTTONUP);
}


void EventManager::process() {
	while (SDL_PollEvent(&e)) {
		#ifndef NDEBUG
		std::string eventStr = EventToString(e);
		if (!eventStr.empty())
			std::cout << eventStr << std::endl;
		#endif
		switch (e.type) {
		case SDL_MOUSEMOTION:
		case SDL_MOUSEBUTTONDOWN:
		case SDL_MOUSEBUTTONUP:
		case SDL_KEYDOWN:
		case SDL_KEYUP:
		case SDL_TEXTINPUT:
		case SDL_TEXTEDITING:
			inputHandler.process(e);
			break;
		case SDL_QUIT:
			SDL::clearEvents();
			GameData::instance().stateManager->exit();
			break;
		default:
			inputHandler.getCallbacks()->eventCallback(e);
			break;
		}
	}
}


#ifndef NDEBUG

// helper for EventToString
static void MouseEventToString(std::stringstream& ss, const SDL_Event& e) {
	if (e.type == SDL_MOUSEBUTTONDOWN) {
		#if !defined(DEBUG_EVENT_MOUSEBUTTONDOWN) || !DEBUG_EVENT_MOUSEBUTTONDOWN
			return;
		#endif
		ss << DEBUG_EVENT_PREPEND << "MOUSEBUTTON " << "DOWN";
	}
	else if (e.type == SDL_MOUSEBUTTONUP) {
		#if !defined(DEBUG_EVENT_MOUSEBUTTONUP) || !DEBUG_EVENT_MOUSEBUTTONUP
			return;
		#endif
		ss << DEBUG_EVENT_PREPEND << "MOUSEBUTTON " << "  UP";
	}
	else
		assert(false);
	ss << ": time=" << e.button.timestamp << " pos="
	   << std::setfill(' ') << std::setw(3) << e.button.x << ", "
	   << std::setfill(' ') << std::setw(3) << e.button.y
	   << " button=";
	switch (e.button.button) {
	case SDL_BUTTON_LEFT:
		ss << "LEFT";
		break;
	case SDL_BUTTON_MIDDLE:
		ss << "MIDDLE";
		break;
	case SDL_BUTTON_RIGHT:
		ss << "RIGHT";
		break;
	case SDL_BUTTON_X1:
		ss << "X1";
		break;
	case SDL_BUTTON_X2:
		ss << "X2";
		break;
	default:
		ss << '?';
	}
	ss << " clicks=" << static_cast<int>(e.button.clicks);
}


static std::string EventToString(const SDL_Event& e) {
	std::stringstream ss;
	switch (e.type) {
	case SDL_MOUSEBUTTONDOWN:
	case SDL_MOUSEBUTTONUP:
		MouseEventToString(ss, e);
		break;
	case SDL_TEXTINPUT:
		#if defined(DEBUG_EVENT_TEXTINPUT) && DEBUG_EVENT_TEXTINPUT
			ss << DEBUG_EVENT_PREPEND << "TEXTINPUT: time=" << e.text.timestamp << " text=\"" << e.text.text << "\"";
		#endif
		break;
	case SDL_TEXTEDITING:
		#if defined(DEBUG_EVENT_TEXTEDITING) && DEBUG_EVENT_TEXTEDITING
			ss << DEBUG_EVENT_PREPEND << "TEXTEDITING: time=" << e.edit.timestamp << " text=\"" << e.edit.text
			   << "\" start=" << e.edit.start << " length=" << e.edit.length;
		#endif
		break;
	case SDL_QUIT:
		#if defined(DEBUG_EVENT_QUIT) && DEBUG_EVENT_QUIT
			ss << DEBUG_EVENT_PREPEND << "QUIT: time=" << e.quit.timestamp;
		#endif
		break;
	case SDL_WINDOWEVENT:
		#if defined(DEBUG_EVENT_WINDOWEVENT) && DEBUG_EVENT_WINDOWEVENT
			ss << DEBUG_EVENT_PREPEND << "WINDOWEVENT: time=" << e.window.timestamp << " id=" << e.window.windowID << " event=";
			switch (e.window.event) {
			case SDL_WINDOWEVENT_SHOWN:
				ss << "SHOWN";
				break;
			case SDL_WINDOWEVENT_HIDDEN:
				ss << "HIDDEN";
				break;
			case SDL_WINDOWEVENT_EXPOSED:
				ss << "EXPOSED";
				break;
			case SDL_WINDOWEVENT_MOVED:
				ss << "MOVED to " << e.window.data1 << ',' << e.window.data2;
				break;
			case SDL_WINDOWEVENT_RESIZED:
				ss << "RESIZED to " << e.window.data1 << 'x' << e.window.data2;;
				break;
			case SDL_WINDOWEVENT_SIZE_CHANGED:
				ss << "SIZE_CHANGED";
				break;
			case SDL_WINDOWEVENT_MINIMIZED:
				ss << "MINIMIZED";
				break;
			case SDL_WINDOWEVENT_MAXIMIZED:
				ss << "MAXIMIZED";
				break;
			case SDL_WINDOWEVENT_RESTORED:
				ss << "RESTORED";
				break;
			case SDL_WINDOWEVENT_ENTER:
				ss << "ENTER";
				break;
			case SDL_WINDOWEVENT_LEAVE:
				ss << "LEAVE";
				break;
			case SDL_WINDOWEVENT_FOCUS_GAINED:
				ss << "FOCUS_GAINED";
				break;
			case SDL_WINDOWEVENT_FOCUS_LOST:
				ss << "FOCUS_LOST";
				break;
			case SDL_WINDOWEVENT_CLOSE:
				ss << "CLOSE";
				break;
			}
		#endif
		break;
	default:
		if (e.type >= SDL_USEREVENT) {
			#if defined(DEBUG_EVENT_USEREVENT) && DEBUG_EVENT_USEREVENT
				ss << DEBUG_EVENT_PREPEND << "USEREVENT: time=" << e.user.timestamp << " code=" << e.user.code;
			#endif
		}
		else {
			// ignore event
		}
	}
	return ss.str();
}

#endif
