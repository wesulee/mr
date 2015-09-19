#include "widget_text.h"
#include "canvas.h"
#include "constants.h"
#include "game_data.h"
#include "text_renderer.h"
#include "widget_data.h"
#include <algorithm>	// min
#include <cassert>


WidgetText::WidgetText() : tex(nullptr), texSz(0, 0), colText(COLOR_WHITE) {
	sizePolicy = WidgetSizePolicy::PREFER;
	tr = GameData::instance().wData.defaultTR;
}


WidgetText::~WidgetText() {
	SDL::freeNull(tex);
}


void WidgetText::setRenderer(TextRenderer* p) {
	assert(p != nullptr);
	tr = p;
	render();
}


void WidgetText::setText(const std::string& str) {
	text = str;
	render();
	_doResize(IntPair{bounds.w, texSz.second});
}


void WidgetText::setTextColor(const Color& c) {
	colText = c;
	render();
}


void WidgetText::enableBackground() {
	drawBg = true;
}


void WidgetText::enableBackground(const Color& c) {
	drawBg = true;
	colBg = c;
}


void WidgetText::disableBackground() {
	drawBg = false;
}


void WidgetText::draw(Canvas& can) {
	SDL_Rect r{0, 0, bounds.w, bounds.h};
	if (drawBg) {
		can.setColor(colBg);
		can.fillRect(r);
	}
	if (tex != nullptr) {
		r.w = std::min(bounds.w, texSz.first);
		r.h = std::min(bounds.h, texSz.second);
		can.draw(tex, &r);
	}
}


void WidgetText::event(WidgetEvent&) {
	// do nothing
}


IntPair WidgetText::getPrefSize() const {
	return IntPair{Constants::WSizeExpand, texSz.second};
}


IntPair WidgetText::getMinSize() const {
	if (tex != nullptr)
		return IntPair{texSz.first, texSz.second};
	else
		return IntPair{1, 1};		// arbitrary
}


void WidgetText::_resize(const IntPair& p, const WidgetResizeFlag f) {
	bounds.w = p.first;
	bounds.h = p.second;
	switch (f) {
	case WidgetResizeFlag::PARENT:
		render();
		break;
	default:
		break;
	}
}


void WidgetText::render() {
	if (tex != nullptr) {
		SDL::free(tex);
		tex = nullptr;
		texSz = IntPair{0, 0};
	}
	if (text.empty())
		return;
	tr->setColor(colText);
	assert(bounds.w > 0);
	SDL_Surface* surf = tr->renderWrap(text, bounds.w);
	texSz.first = surf->w;
	texSz.second = surf->h;
	tex = SDL::toTexture(surf);
}
