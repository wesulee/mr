#include "widget_scroll_bar.h"
#include "canvas.h"
#include "shapes.h"
#include "widget_event.h"
#include <algorithm>
#include <cassert>


void AbstractScrollBar::setWindowSize(const int n) {
	assert(n > 0);
	windowSize = n;
	if (contentSize < windowSize)
		contentSize = windowSize;
	updateGripSize();
}


void AbstractScrollBar::setContentSize(const int n) {
	assert(n > 0);
	contentSize = std::max(n, windowSize);
	updateGripSize();
}


void AbstractScrollBar::setGripOffset(int n) {
	if (n < 0)
		n = 0;
	else if (n > getMaxGripOffset())
		n = getMaxGripOffset();
	gripOffset = n;
	// make sure on max grip offset has contentOffset see very end
	if (gripOffset == getMaxGripOffset())
		contentOffset = contentSize - windowSize;
	else
		contentOffset = static_cast<decltype(contentOffset)>(ratioCG * gripOffset);
}


// called whenever windowSize or contentSize has been changed
void AbstractScrollBar::updateGripSize() {
	if (gripSize > windowSize) {
		gripSize = windowSize;
	}
	if (windowSize == contentSize) {
		gripSize = windowSize;
		ratioCG = 0;
	}
	else {
		gripSize = std::max(
			static_cast<decltype(gripSize)>(static_cast<float>(windowSize) / contentSize * windowSize),
			minGripSize
		);
		ratioCG = static_cast<decltype(ratioCG)>(contentSize - windowSize) / getMaxGripOffset();
	}
}


ScrollBar::ScrollBar() {
	bounds.w = 20;
	bounds.h = 100;
	barBounds.x = 0;
	bar.setWindowSize(bounds.h);
	updateBarBounds();
}


void ScrollBar::setContentSize(const int n) {
	bar.setContentSize(n);
	updateBarBounds();
}


void ScrollBar::draw(Canvas& can) {
	can.setColor(style.bg);
	can.fillRect(0, 0, bounds.w, bounds.h);
	switch (state) {
	case WidgetState::OUT:
		can.setColor(style.out);
		break;
	case WidgetState::OVER:
		can.setColor(style.over);
		break;
	case WidgetState::DOWN:
		can.setColor(style.down);
		break;
	}
	can.fillRect(barBounds);
}


void ScrollBar::event(WidgetEvent& e) {
	WEMouseAutoUpdate mTest;
	mTest.set(e, bounds);
	switch (e.type) {
	case WidgetEventType::MOUSE_MOVE:
		if (state == WidgetState::DOWN) {
			// even while mouse is outside bar, continue to update offset while pressed
			bar.setGripOffset(e.move.y + pressOffset);
			barBounds.y = bar.getGripOffset();
			if (bar.getContentOffset() != prevContentOffset) {
				prevContentOffset = bar.getContentOffset();
				callback(prevContentOffset);
			}
		}
		else {
			if (Shape::contains(barBounds, e.move.x, e.move.y))
				state = WidgetState::OVER;
			else
				state = WidgetState::OUT;
		}
		break;
	case WidgetEventType::MOUSE_PRESS:
		if (Shape::contains(barBounds, e.press.x, e.press.y)) {
			state = WidgetState::DOWN;
			pressOffset = -e.press.y + bar.getGripOffset();
			prevContentOffset = bar.getContentOffset();
		}
		else
			state = WidgetState::OUT;
		break;
	case WidgetEventType::MOUSE_RELEASE:
		if (Shape::contains(barBounds, e.release.x, e.release.y))
			state = WidgetState::OVER;
		else
			state = WidgetState::OUT;
		break;
	default:
		break;
	}
}


IntPair ScrollBar::getPrefSize() const {
	return getSize();
}


IntPair ScrollBar::getMinSize() const {
	return IntPair{1, bar.getMinSize()};
}


void ScrollBar::_resize(const IntPair& p, const WidgetResizeFlag) {
	assert(getContentOffset() == 0);
	assert((p.first > 0) && (p.second > 0));
	bounds.w = p.first;
	bounds.h = p.second;
	bar.setWindowSize(bounds.h);
	updateBarBounds();
}


void ScrollBar::updateBarBounds() {
	barBounds.y = bar.getGripOffset();
	barBounds.w = bounds.w;
	barBounds.h = bar.getGripSize();
}
