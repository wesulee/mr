#include "widget_area.h"
#include "canvas.h"
#include "exception.h"
#include "logger.h"
#include "widget_event.h"
#include "widget_layout.h"
#include <cassert>


namespace WidgetAreaHelper {
	constexpr char errorMessage[] = "WidgetArea invalid method";
}



WidgetArea::WidgetArea() {
	bounds.x = 0;
	bounds.y = 0;
	bounds.w = 100;
	bounds.h = 100;
}


WidgetArea::~WidgetArea() {
	if (layout != nullptr)
		delete layout;
}


void WidgetArea::setPosition(const IntPair& p) {
	assert((p.first >= 0) && (p.second >= 0));
	bounds.x = p.first;
	bounds.y = p.second;
}


void WidgetArea::setSize(const IntPair& p) {
	assert((p.first >= 0) && (p.second >= 0));
	bounds.w = p.first;
	bounds.h = p.second;
	if (layout != nullptr)
		layout->_resize(p, WidgetResizeFlag::PARENT);
}


void WidgetArea::setLayout(WidgetLayout* p) {
	assert(p != nullptr);
	assert(layout == nullptr);	// should not overwrite existing layout
	layout = p;
	layout->_setParent(this);
	IntPair pair{0, 0};
	layout->_setPos(pair);
	pair = getSize();
	layout->_resize(pair, WidgetResizeFlag::PARENT);
	assert(layout->getSize() == getSize());
	layout->setSizePolicy(WidgetSizePolicy::FIXED);
}



void WidgetArea::draw(Canvas& can) {
	can.setViewport(bounds);
	layout->draw(can);
	can.clearViewport();
}


void WidgetArea::event(WidgetEvent& e) {
	if (isMouseEvent(e)) {
		adjust(e, bounds);
		layout->event(e);
		undo(e, bounds);
	}
	else {
		layout->event(e);
	}
}


IntPair WidgetArea::getPrefSize() const {
	assert(false);
	Logger::instance().exit(RuntimeError{WidgetAreaHelper::errorMessage});
	return IntPair{0, 0};
}


IntPair WidgetArea::getMinSize() const {
	assert(false);
	Logger::instance().exit(RuntimeError{WidgetAreaHelper::errorMessage});
	return IntPair{0, 0};
}


void WidgetArea::_requestResize(Widget* w, const IntPair& p) {
	// contained layout must be size of this widget
	w->_resize(p, WidgetResizeFlag::PARENT);
	assert(w->getSize() == p);
}


void WidgetArea::_resize(const IntPair&, const WidgetResizeFlag) {
	assert(false);
	Logger::instance().exit(RuntimeError{WidgetAreaHelper::errorMessage});
}
