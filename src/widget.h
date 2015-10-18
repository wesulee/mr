#pragma once

#include "sdl_header.h"		// SDL_Rect
#include "utility_struct.h"		// IntPair
#include <cassert>
#include <cstddef>
#include <limits>


class Canvas;
struct WidgetEvent;


enum class WidgetAlignmentHoriz {LEFT, CENTER, RIGHT};
enum class WidgetAlignmentVert {TOP, CENTER, BOTTOM};
// WidgetSizePolicy is used by parent to determine the size of child.
// NONE: no preferred size, so let parent determine size
// PREFER: prefer to use the size of getPrefSize(), or as close to it
// FIXED: the size should never be modified by parent
enum class WidgetSizePolicy {NONE, PREFER, FIXED};
// WidgetResizeFlag is parameter for _resize(), used to simplify logic for resizing.
// NONE: default value
// SELF: called directly on itself
// PARENT: parent on its own
// REQUEST_RESIZE: resize is result of call to _requestResize()
enum class WidgetResizeFlag {NONE, SELF, PARENT, REQUEST_RESIZE};
enum class WidgetState {OUT, OVER, DOWN};


/*
Methods that begin with _ should only be called by other widgets.
Widget::bounds determines size and position. Position is always relative to its parent.
WidgetArea must be the root widget.
Resizing: A Widget with a parent is not allowed to resize itself. This must be done through child
  calling requestSize() on parent, where parent then calls _resize() on child with any value.
  Parent may also call _resize() on child at any time.
Drawing: Widgets can assume viewport has been set on the bounds of itself (so parent must set
  viewport prior to calling draw() on child).
Event handling: Widget may assume mouse offsets are relative to its parent, so a mouse event can
  be checked if it is contained in its own bounds.
Widgets with children:
  -must implement _requestResize()
  -must set viewport around each child before drawing it
  -must adjust event mouse coordinates to make it relative to itself
Widgets without children:
  -must NOT implement _requestResize()
In general, a widget's size policy should be PREFER.
Layouts may make the assumption that after a widget's _resize(), getMinSize() will always return
  the same value until after the next _resize(). Once a widget has been added, the size policy is
  assumed to never change.
WSizeExpand may only be used (as parameter or return) in getPrefSize() and _requestResize().
When IntPair used as a size, first is width, second is height.
*/
class Widget {
public:
	Widget();
	Widget(Widget* const);
	virtual ~Widget() {}
	virtual void draw(Canvas&) = 0;
	virtual void event(WidgetEvent&);
	virtual IntPair getPrefSize(void) const = 0;	// preferred size of the widget
	virtual IntPair getMinSize(void) const = 0;		// minimum size that widget can be resized to
	virtual void setSizePolicy(const WidgetSizePolicy);
	WidgetSizePolicy getSizePolicy(void) const;
	virtual IntPair getSize(void) const;
	const SDL_Rect& getBounds(void) const;
	virtual IntPair getPos(void) const;
	// methods below this should only be called by other widgets
	virtual void _setPos(const IntPair&);
	virtual void _requestResize(Widget*, const IntPair&);
	virtual void _resize(const IntPair&, const WidgetResizeFlag) = 0;
	void _doResize(const IntPair&);
	void _setParent(Widget*);
	Widget* _getParent(void);
	const Widget* _getParent(void) const;
	void _setIndex(const std::size_t);
	std::size_t _getIndex(void) const;
protected:
	SDL_Rect bounds;
	WidgetSizePolicy sizePolicy = WidgetSizePolicy::NONE;
private:
	Widget* parent;
	// maintained by parent, used mainly by layouts
	std::size_t idx = std::numeric_limits<std::size_t>::max();
};


// When not visible, safe to assume that
// draw, event will never be called
class WidgetWithVisibility : public Widget {
public:
	WidgetWithVisibility() {}
	virtual ~WidgetWithVisibility() {}
	virtual void setVisible(const bool);
	bool isVisible(void) const;
protected:
	bool visible = false;
};


inline
Widget::Widget() : Widget(nullptr) {
}


inline
Widget::Widget(Widget* const p) : parent(p) {
	// default bounds
	bounds.x = 0;
	bounds.y = 0;
	bounds.w = 50;
	bounds.h = 50;
}


inline
void Widget::event(WidgetEvent&) {
	return;		// ignore by default, since some widgets are not interactive
}


inline
void Widget::setSizePolicy(const WidgetSizePolicy p) {
	sizePolicy = p;
}


inline
WidgetSizePolicy Widget::getSizePolicy() const {
	return sizePolicy;
}


inline
IntPair Widget::getSize() const {
	return IntPair{bounds.w, bounds.h};
}


inline
const SDL_Rect& Widget::getBounds() const {
	return bounds;
}


inline
IntPair Widget::getPos() const {
	return IntPair{bounds.x, bounds.y};
}


inline
void Widget::_setPos(const IntPair& p) {
	bounds.x = p.first;
	bounds.y = p.second;
}


inline
void Widget::_requestResize(Widget*, const IntPair&) {
	// since many Widgets do not have child, implement default that does nothing
	return;
}


inline
void Widget::_doResize(const IntPair& p) {
	assert((p.first >= 0) && (p.second >= 0));
	if (_getParent() == nullptr)
		_resize(p, WidgetResizeFlag::SELF);
	else
		_getParent()->_requestResize(this, p);
}


inline
void Widget::_setParent(Widget* p) {
	parent = p;
}


inline
Widget* Widget::_getParent() {
	return parent;
}


inline
const Widget* Widget::_getParent() const {
	return parent;
}


inline
void Widget::_setIndex(const std::size_t i) {
	idx = i;
}


inline
std::size_t Widget::_getIndex() const {
	return idx;
}


inline
void WidgetWithVisibility::setVisible(const bool b) {
	visible = b;
}


inline
bool WidgetWithVisibility::isVisible() const {
	return visible;
}
