#include "widget_progress_bar.h"
#include "canvas.h"
#include "constants.h"
#include "utility.h"
#include <cassert>


ProgressBar::ProgressBar() {
	// default values
	prefSize.first = static_cast<int>(Constants::windowWidth * 0.33f);
	prefSize.second = static_cast<int>(Constants::windowHeight * 0.15f);
	resetPos();
}


void ProgressBar::setPrefSize(const IntPair& p) {
	assert((p.first > 0) && (p.second > 0));
	prefSize = p;
}


void ProgressBar::setBackgroundColor(const Color& c) {
	colBg = c;
}


void ProgressBar::setFillColor(const Color& c) {
	colFill = c;
}


void ProgressBar::setOutlineSize(const int n) {
	assert(n >= 0);
	outlineSz = n;
	resetPos();
}


void ProgressBar::setMaxValue(const int v) {
	assert(v > 0);
	maxValue = v;
	setProgBounds();
}


void ProgressBar::setValue(const int v) {
	assert((v >= 0) && (v <= maxValue));
	if (v < 0)
		value = 0;
	else if (v > maxValue)
		value = maxValue;
	else
		value = v;
	setProgBounds();
}


void ProgressBar::draw(Canvas& can) {
	if (outlineSz != 0) {
		can.setColor(colBg);
		can.fillRect(bounds);
		can.setColor(colFill);
		can.fillRect(progBounds);
	}
	else {
		can.setColor(colFill);
		can.fillRect(bounds);
		can.setColor(colBg);
		shrinkRect(bounds, outlineSz);
		can.fillRect(bounds);
		shrinkRect(bounds, -outlineSz);
		can.setColor(colFill);
		can.fillRect(progBounds);
	}
}


IntPair ProgressBar::getPrefSize() const {
	return prefSize;
}


IntPair ProgressBar::getMinSize() const {
	return IntPair{1, 1};
}


void ProgressBar::_resize(const IntPair& p, const WidgetResizeFlag) {
	assert((p.first > 0) && (p.second > 0));
	bounds.w = p.first;
	bounds.h = p.second;
	resetPos();
}


void ProgressBar::resetPos() {
	progBounds.x = bounds.x + outlineSz;
	progBounds.y = bounds.y + outlineSz;
	progBounds.h = bounds.h - (outlineSz * 2);
	setProgBounds();
}


void ProgressBar::setProgBounds() {
	if (done())
		progBounds.w = bounds.w - (outlineSz * 2);
	else {
		progBounds.w = static_cast<int>(
			(static_cast<float>(value) / maxValue)
			* (bounds.w - (outlineSz * 2))
		);
	}
}
