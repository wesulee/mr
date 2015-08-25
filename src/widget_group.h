#pragma once

#include "sdl_header.h"
#include <functional>


class Widget;
class WidgetEvent;


// Handles widget events
class LayoutWidgetGroup {
public:
	typedef std::function<Widget*(const int, const int)> LookupFunction;
	void setBounds(SDL_Rect*);
	void setLookupFunc(LookupFunction);
	void event(WidgetEvent&);
private:
	LookupFunction lookup;
	Widget* selected = nullptr;
	SDL_Rect* bounds = nullptr;
	bool selectedExclusive = false;
};


inline
void LayoutWidgetGroup::setBounds(SDL_Rect* r) {
	bounds = r;
}


inline
void LayoutWidgetGroup::setLookupFunc(LookupFunction func) {
	lookup = func;
}
