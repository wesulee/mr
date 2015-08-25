#pragma once

#include "sdl_header.h"
#include <utility>


struct WidgetEvent;
struct WidgetEventMouseMove;
struct WidgetEventMousePress;
struct WidgetEventMouseRelease;
struct WidgetEventMouseClick;
struct WidgetEventTextInput;
struct WidgetEventTextEdit;
struct WidgetEventTextDelete;
// prototype
bool operator==(const WidgetEvent&, const WidgetEvent&);
bool operator==(const WidgetEventMouseMove&, const WidgetEventMouseMove&);
bool operator==(const WidgetEventMousePress&, const WidgetEventMousePress&);
bool operator==(const WidgetEventMouseRelease&, const WidgetEventMouseRelease&);
bool operator==(const WidgetEventMouseClick&, const WidgetEventMouseClick&);
bool operator==(const WidgetEventTextInput&, const WidgetEventTextInput&);
bool operator==(const WidgetEventTextEdit&, const WidgetEventTextEdit&);
bool operator==(const WidgetEventTextDelete&, const WidgetEventTextDelete&);
bool containsWidgetRect(const SDL_Rect&, const WidgetEvent&);
bool containsWidgetRect(const SDL_Rect&, const int, const int);
bool isMouseEvent(const WidgetEvent&);
void adjust(WidgetEvent&, const SDL_Rect&);
void undo(WidgetEvent&, const SDL_Rect&);
void weAdjustMouse(WidgetEvent&, const int, const int);
std::pair<int*, int*> getMousePos(WidgetEvent&);
std::pair<const int*, const int*> getMousePos(const WidgetEvent&);


enum class WidgetEventType {MOUSE_MOVE, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_CLICK, TEXT_INPUT, TEXT_EDIT, TEXT_DELETE};


class WEMouseAutoUpdate {
public:
	WEMouseAutoUpdate() = default;
	~WEMouseAutoUpdate();
	bool set(WidgetEvent&, const SDL_Rect&);	// is mouse event?
	bool inBounds(void) const;		// mouse event and inside bounds?
	std::pair<int, int> getPos(void) const;	// get relative mouse coordinates
private:
	WidgetEvent* event = nullptr;
	int x;
	int y;
	bool contains = false;
};


struct WidgetEventMouseMove {
	int x;
	int y;
};


struct WidgetEventMousePress {
	int x;
	int y;
};


struct WidgetEventMouseRelease {
	int x;
	int y;
	unsigned int time;	// ms, since most recent press
};


struct WidgetEventMouseClick {
	int x;
	int y;
};


struct WidgetEventTextInput {
	// the null-terminated input text in UTF-8 encoding
	char text[32];
};


struct WidgetEventTextEdit {
	//! not implemented
};


struct WidgetEventTextDelete {
	bool dirLeft;	// true when backspace, false when delete
	bool modifier;	// ctrl pressed?
};


struct WidgetEvent {
	union {
		WidgetEventMouseMove move;
		WidgetEventMousePress press;
		WidgetEventMouseRelease release;
		WidgetEventMouseClick click;
		WidgetEventTextInput textInput;
		WidgetEventTextEdit textEdit;
		WidgetEventTextDelete textDelete;
	};
	WidgetEventType type;
};


// correct mouse coordinates to make relative to rect
inline
void adjust(WidgetEvent& e, const SDL_Rect& rect) {
	weAdjustMouse(e, rect.x, rect.y);
}


// undo adjust()
inline
void undo(WidgetEvent& e, const SDL_Rect& rect) {
	weAdjustMouse(e, -rect.x, -rect.y);
}


inline
WEMouseAutoUpdate::~WEMouseAutoUpdate() {
	if (event != nullptr)
		weAdjustMouse(*event, -x, -y);
}


inline
bool WEMouseAutoUpdate::inBounds() const {
	return contains;
}


#ifndef NDEBUG
#include <ostream>

std::ostream& operator<<(std::ostream&, const WidgetEventMouseMove&);
std::ostream& operator<<(std::ostream&, const WidgetEventMousePress&);
std::ostream& operator<<(std::ostream&, const WidgetEventMouseRelease&);
std::ostream& operator<<(std::ostream&, const WidgetEventMouseClick&);
std::ostream& operator<<(std::ostream&, const WidgetEventTextInput&);
std::ostream& operator<<(std::ostream&, const WidgetEventTextEdit&);
std::ostream& operator<<(std::ostream&, const WidgetEventTextDelete&);
#endif
