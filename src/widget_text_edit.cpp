#include "widget_text_edit.h"
#include "canvas.h"
#include "constants.h"
#include "game_data.h"
#include "input_handler.h"
#include "shapes.h"
#include "text_renderer.h"
#include "widget_data.h"
#include "widget_event.h"
#include <algorithm>
#include <cassert>


namespace TextEditSettings {
	constexpr int outlineSz = 1;
}


TextEdit::TextEdit() : offsetX(3 + TextEditSettings::outlineSz) {
	sizePolicy = WidgetSizePolicy::PREFER;
	texBounds.x = offsetX;
	setRenderer(GameData::instance().wData.defaultTR);
}


TextEdit::~TextEdit() {
	// do not delete tr
	SDL::freeNull(tex);
	disableTextInput();
}


void TextEdit::setRenderer(TextRenderer* const p) {
	tr = p;
	if (sizePolicy != WidgetSizePolicy::FIXED) {
		if (_getParent() != nullptr)
			_getParent()->_requestResize(this, getPrefSize());
		else
			bounds.h = tr->getMetrics().height + (paddingV * 2);
	}
}


void TextEdit::setOffset(const int x) {
	assert(x >= 0);
	offsetX = x + TextEditSettings::outlineSz;
}


void TextEdit::setVertPadding(const int n) {
	assert(n >= 0);
	paddingV = n;
	bounds.h = tr->getMetrics().height + (paddingV * 2);
}


void TextEdit::setStyle(const Color& text, const Color& bg, const Color& outline) {
	colText = text;
	colBg = bg;
	colOutline = outline;
}


void TextEdit::setText(const std::string& str) {
	strText = str;
	updateText();
}


void TextEdit::draw(Canvas& can) {
	using namespace TextEditSettings;
	can.setColor(colOutline);
	can.fillRect(0, 0, bounds.w, bounds.h);
	can.setColor(colBg);
	can.fillRect(
		outlineSz,
		outlineSz,
		bounds.w - (outlineSz * 2),
		bounds.h - (outlineSz * 2)
	);
	if (tex != nullptr)
		can.draw(tex, &texBounds);
}


void TextEdit::event(WidgetEvent& e) {
	switch (e.type) {
	case WidgetEventType::MOUSE_PRESS:
		if (Shape::contains(bounds, e.press.x, e.press.y))
			enableTextInput();
		else
			disableTextInput();
		break;
	case WidgetEventType::TEXT_INPUT:
		strText += e.textInput.text;
		updateText();
		break;
	case WidgetEventType::TEXT_EDIT:
		break;
	case WidgetEventType::TEXT_DELETE:
		if (e.textDelete.dirLeft && !strText.empty()) {
			strText.pop_back();
			updateText();
		}
		break;
	default:
		break;
	}
}


IntPair TextEdit::getPrefSize() const {
	return IntPair{
		Constants::WSizeExpand,
		tr->getMetrics().height + (paddingV * 2)
	};
}


IntPair TextEdit::getMinSize() const {
	return IntPair{
		(TextEditSettings::outlineSz * 2) + 10,
		(TextEditSettings::outlineSz * 2) + 5
	};
}


void TextEdit::_resize(const IntPair& p, const WidgetResizeFlag) {
	assert((p.first > 0) && (p.second > 0));
	bounds.w = p.first;
	bounds.h = p.second;
	if (tex != nullptr) {
		texBounds.y = ((bounds.h - texBounds.h) / 2);
	}
}


void TextEdit::updateText() {
	if (strText.empty()) {
		SDL::freeNull(tex);
		tex = nullptr;
	}
	else {
		tr->setColor(colText);
		SDL_Surface* surf = tr->render(strText);
		SDL::freeNull(tex);
		if (surf == nullptr) {
			tex = nullptr;
		}
		else {
			texBounds.w = surf->w;
			texBounds.h = surf->h;
			texBounds.y = ((bounds.h - texBounds.h) / 2);
			tex = SDL::toTexture(surf);
		}
	}
}


void TextEdit::enableTextInput() {
	GameData::instance().inputHandler->setTextInputWidget(this);
	GameData::instance().inputHandler->startTextInput();
}


void TextEdit::disableTextInput() {
	GameData::instance().inputHandler->stopTextInput();
}
