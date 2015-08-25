#include "widget_event.h"
#include "shapes.h"
#include <cassert>
#include <cstring>	// strcmp
#include <limits>


bool operator==(const WidgetEvent& e1, const WidgetEvent& e2) {
	if (e1.type != e2.type)
		return false;
	switch (e1.type) {
	case WidgetEventType::MOUSE_MOVE:
		return (e1.move == e2.move);
	case WidgetEventType::MOUSE_PRESS:
		return (e1.press == e2.press);
	case WidgetEventType::MOUSE_RELEASE:
		return (e1.release == e2.release);
	case WidgetEventType::MOUSE_CLICK:
		return (e1.click == e2.click);
	case WidgetEventType::TEXT_INPUT:
		return (e1.textInput == e2.textInput);
	case WidgetEventType::TEXT_EDIT:
		return (e1.textEdit == e2.textEdit);
	case WidgetEventType::TEXT_DELETE:
		return (e1.textDelete == e2.textDelete);
	}
	assert(false);
	return false;
}


bool operator==(const WidgetEventMouseMove& e1, const WidgetEventMouseMove& e2) {
	return (
		(e1.x == e2.x)
		&& (e1.y == e2.y)
	);
}


bool operator==(const WidgetEventMousePress& e1, const WidgetEventMousePress& e2) {
	return (
		(e1.x == e2.x)
		&& (e1.y == e2.y)
	);
}


bool operator==(const WidgetEventMouseRelease& e1, const WidgetEventMouseRelease& e2) {
	return (
		(e1.x == e2.x)
		&& (e1.y == e2.y)
		&& (e1.time == e2.time)
	);
}


bool operator==(const WidgetEventMouseClick& e1, const WidgetEventMouseClick& e2) {
	return (
		(e1.x == e2.x)
		&& (e1.y == e2.y)
	);
}


bool operator==(const WidgetEventTextInput& e1, const WidgetEventTextInput& e2) {
	return (std::strcmp(e1.text, e2.text) == 0);
}


bool operator==(const WidgetEventTextEdit&, const WidgetEventTextEdit&) {
	assert(false);	//! not implemented
	return false;
}


bool operator==(const WidgetEventTextDelete& e1, const WidgetEventTextDelete& e2) {
	return (
		(e1.dirLeft == e2.dirLeft)
		&& (e1.modifier == e2.modifier)
	);
}


bool containsWidgetRect(const SDL_Rect& r, const WidgetEvent& e) {
	auto pos = getMousePos(e);
	return containsWidgetRect(r, *pos.first, *pos.second);
}


bool containsWidgetRect(const SDL_Rect& r, const int x, const int y) {
	return ((x >= 0) && (x < r.w) && (y >= 0) && (y < r.h));
}


bool isMouseEvent(const WidgetEvent& e) {
	switch (e.type) {
	case WidgetEventType::MOUSE_MOVE:
	case WidgetEventType::MOUSE_PRESS:
	case WidgetEventType::MOUSE_RELEASE:
	case WidgetEventType::MOUSE_CLICK:
		return true;
	default:
		return false;
	}
}


void weAdjustMouse(WidgetEvent& e, const int subX, const int subY) {
	auto pos = getMousePos(e);
	*pos.first -= subX;
	*pos.second -= subY;
}


std::pair<int*, int*> getMousePos(WidgetEvent& e) {
	std::pair<int*, int*> pos;
	switch (e.type) {
	case WidgetEventType::MOUSE_MOVE:
		pos.first = &e.move.x;
		pos.second = &e.move.y;
		break;
	case WidgetEventType::MOUSE_PRESS:
		pos.first = &e.press.x;
		pos.second = &e.press.y;
		break;
	case WidgetEventType::MOUSE_RELEASE:
		pos.first = &e.release.x;
		pos.second = &e.release.y;
		break;
	case WidgetEventType::MOUSE_CLICK:
		pos.first = &e.click.x;
		pos.second = &e.click.y;
		break;
	default:
		assert(false);
		break;
	}
	return pos;
}


std::pair<const int*, const int*> getMousePos(const WidgetEvent& e) {
	std::pair<const int*, const int*> pos;
	switch (e.type) {
	case WidgetEventType::MOUSE_MOVE:
		pos.first = &e.move.x;
		pos.second = &e.move.y;
		break;
	case WidgetEventType::MOUSE_PRESS:
		pos.first = &e.press.x;
		pos.second = &e.press.y;
		break;
	case WidgetEventType::MOUSE_RELEASE:
		pos.first = &e.release.x;
		pos.second = &e.release.y;
		break;
	case WidgetEventType::MOUSE_CLICK:
		pos.first = &e.click.x;
		pos.second = &e.click.y;
		break;
	default:
		assert(false);
		break;
	}
	return pos;
}


// make mouse coordinates relative to rect
bool WEMouseAutoUpdate::set(WidgetEvent& e, const SDL_Rect& rect) {
	if (isMouseEvent(e)) {
		event = &e;
		x = rect.x;
		y = rect.y;
		auto mPos = getMousePos(e);
		contains = Shape::contains(rect, *mPos.first, *mPos.second);
		*mPos.first -= x;
		*mPos.second -= y;
		return true;
	}
	return false;
}


// get relative mouse coordinates
// if not mouse, then returns max int
std::pair<int, int> WEMouseAutoUpdate::getPos() const {
	std::pair<int, int> ret;
	if (isMouseEvent(*event)) {
		auto mPos = getMousePos(*event);
		ret.first = *mPos.first;
		ret.second = *mPos.second;
	}
	else {
		ret.first = std::numeric_limits<int>::max();
		ret.second = ret.first;
	}
	return ret;
}


#ifndef NDEBUG
#include "constants.h"


std::ostream& operator<<(std::ostream& os, const WidgetEventMouseMove& e) {
	(void)e;
	#if defined(DEBUG_WE_MOUSE_MOVE) && DEBUG_WE_MOUSE_MOVE
		os << DEBUG_WE_PREPEND << "MOUSE_MOVE x=" << e.x << " y=" << e.y;
	#endif
	return os;
}


std::ostream& operator<<(std::ostream& os, const WidgetEventMousePress& e) {
	(void)e;
	#if defined(DEBUG_WE_MOUSE_PRESS) && DEBUG_WE_MOUSE_PRESS
		os << DEBUG_WE_PREPEND << "MOUSE_PRESS x=" << e.x << " y=" << e.y;
	#endif
	return os;
}


std::ostream& operator<<(std::ostream& os, const WidgetEventMouseRelease& e) {
	(void)e;
	#if defined(DEBUG_WE_MOUSE_RELEASE) && DEBUG_WE_MOUSE_RELEASE
		os << DEBUG_WE_PREPEND << "MOUSE_RELEASE x=" << e.x << " y=" << e.y << " time=" << e.time;
	#endif
	return os;
}


std::ostream& operator<<(std::ostream& os, const WidgetEventMouseClick& e) {
	(void)e;
	#if defined(DEBUG_WE_MOUSE_CLICK) && DEBUG_WE_MOUSE_CLICK
		os << DEBUG_WE_PREPEND << "MOUSE_CLICK x=" << e.x << " y=" << e.y;
	#endif
	return os;
}


std::ostream& operator<<(std::ostream& os, const WidgetEventTextInput& e) {
	(void)e;
	#if defined(DEBUG_WE_TEXT_INPUT) && DEBUG_WE_TEXT_INPUT
		os << DEBUG_WE_PREPEND << "TEXT_INPUT text=\"" << e.text << "\"";
	#endif
	return os;
}


std::ostream& operator<<(std::ostream& os, const WidgetEventTextEdit& e) {
	// not implemented
	(void)e;
	return os;
}


std::ostream& operator<<(std::ostream& os, const WidgetEventTextDelete& e) {
	(void)e;
	#if defined(DEBUG_WE_TEXT_DELETE) && DEBUG_WE_TEXT_DELETE
		os << DEBUG_WE_PREPEND << "TEXT_DELETE dirLeft=" << e.dirLeft << " modifier=" << e.modifier;
	#endif
	return os;
}

#endif	// NDEBUG
