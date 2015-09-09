#include "widget_text_item.h"
#include "canvas.h"
#include "exception.h"
#include "logger.h"
#include "sdl_helper.h"
#include "widget_text_list_view.h"
#include <cassert>


TextItem::TextItem() {
	textBounds.x = 0;
}


TextItem::~TextItem() {
	SDL::freeNull(tex);
}


void TextItem::setText(const std::string& str) {
	text = str;
	if (isVisible()) {
		SDL::free(tex);
		setText();
	}
}

void TextItem::draw(Canvas& can) {
	assert(isVisible());
	can.draw(tex, &textBounds);
}


IntPair TextItem::getPrefSize() const {
	assert(false);
	return IntPair{1, 1};	// arbitrary
}


IntPair TextItem::getMinSize() const {
	assert(false);
	return IntPair{1, 1};	// arbitrary
}


void TextItem::setVisible(const bool b) {
	if (b == isVisible())
		return;
	const int val = ((static_cast<int>(b) << 1) | static_cast<int>(isVisible()));
	if (val == 1) {			// 0b01
		SDL::free(tex);
		tex = nullptr;
	}
	else if (val == 2) {	// 0b10
		setText();
	}
	else {
		assert(false);
	}
	WidgetWithVisibility::setVisible(b);
}


void TextItem::_resize(const IntPair& p, const WidgetResizeFlag) {
	assert((p.first > 0) && (p.second > 0));
	bounds.w = p.first;
	bounds.h = p.second;
	if (tex != nullptr) {
		// reset textBounds
		textBounds.y = (bounds.h - textBounds.h) / 2;
	}
}


void TextItem::setText() {
	SDL_Surface* surf = getParent2()->renderText(text);
	textBounds.w = surf->w;
	textBounds.h = surf->h;
	textBounds.y = bounds.y + (bounds.h - surf->h) / 2;
	tex = SDL::toTexture(surf);
}


TextListView* TextItem::getParent2() {
	assert(_getParent() != nullptr);
	TextListView* ret;
	if ((_getParent() == nullptr) || ((ret = dynamic_cast<TextListView*>(_getParent())) == nullptr))
		Logger::instance().exit(RuntimeError{"TextItem null or invalid parent"});
	return ret;
}
