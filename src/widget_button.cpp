#include "widget_button.h"
#include "canvas.h"
#include "exception.h"
#include "game_data.h"
#include "shapes.h"
#include "text_renderer.h"
#include "utility.h"
#include "widget_data.h"
#include "widget_event.h"
#include <cassert>
#ifndef NDEBUG
#include "constants.h"
#include <cstdint>
#include <iostream>
#endif


void AbstractButton::event(WidgetEvent& e) {
	switch (e.type) {
	case WidgetEventType::MOUSE_MOVE:
		if (Shape::contains(bounds, e.move.x, e.move.y)) {
			if (pressed)
				state = WidgetState::DOWN;
			else
				state = WidgetState::OVER;
		}
		else
			state = WidgetState::OUT;
		break;
	case WidgetEventType::MOUSE_PRESS:
		if (Shape::contains(bounds, e.press.x, e.press.y)) {
			state = WidgetState::DOWN;
			pressed = true;
			if (cType == CallbackType::ON_PRESS)
				callback();
		}
		else
			pressed = false;
		break;
	case WidgetEventType::MOUSE_RELEASE:
		if (Shape::contains(bounds, e.release.x, e.release.y)) {
			if (pressed && (cType == CallbackType::ON_RELEASE))
				callback();
			state = WidgetState::OVER;
		}
		else
			state = WidgetState::OUT;
		pressed = false;
		break;
	case WidgetEventType::MOUSE_CLICK:
		if (cType == CallbackType::ON_CLICK)
			callback();
		break;
	default:
		break;
	}
}


IntPair TextButtonBase::getPrefSize() const {
	IntPair p;
	if (test()) {
		p = prefSizeOf(IntPair{texBounds.w, texBounds.h});
	}
	else {
		p.first = GameData::instance().wData.buttonDefaultW;
		p.second = GameData::instance().wData.buttonDefaultH;
	}
	return p;
}


IntPair TextButtonBase::getMinSize() const {
	IntPair p;
	if (test()) {
		p.first = texBounds.w;
		p.second = texBounds.h;
	}
	else {
		p.first = 1;
		p.second = 1;
	}
	return p;
}


void TextButtonBase::_resize(const IntPair& p, const WidgetResizeFlag) {
	assert((p.first > 0) && (p.second > 0));
	bounds.w = p.first;
	bounds.h = p.second;
	if (test()) {
		// center texBounds
		texBounds.x = (bounds.w - texBounds.w) / 2;
		texBounds.y = (bounds.h - texBounds.h) / 2;
	}
}


// helper method for derived classes
IntPair TextButtonBase::calcPrefSize(const IntPair& imgDim, const int addH, const int addV) {
	IntPair ret{imgDim.first + (addH * 2), imgDim.second + (addV * 2)};
	const WidgetData& wData = GameData::instance().wData;
	if (ret.first < wData.buttonDefaultW)
		ret.first = wData.buttonDefaultW;
	if (ret.second < wData.buttonDefaultH)
		ret.second = wData.buttonDefaultH;
	return ret;
}


TextButton::TextButton() {
	sizePolicy = WidgetSizePolicy::PREFER;
}


TextButton::~TextButton() {
	SDL::freeNull(tex);
}


void TextButton::setStyle(const std::shared_ptr<Style>& p) {
	style = p;
	if (test())
		_doResize(prefSizeOf(IntPair{texBounds.w, texBounds.h}));
}


void TextButton::setText(const std::string& str) {
	assert(style);
	text = str;
	IntPair prefSz;
	style->tr->setColor(style->colText);
	SDL_Surface* surf = style->tr->render(text);
	texBounds.w = surf->w;
	texBounds.h = surf->h;
	SDL_Texture* nTex = SDL::toTexture(surf);
	prefSz = prefSizeOf(IntPair{texBounds.w, texBounds.h});
	if (test()) {
		SDL::free(tex);
		tex = nTex;
		if (getSize() != prefSz)
			_doResize(prefSz);
	}
	else {
		tex = nTex;
		_doResize(prefSz);
	}
}


void TextButton::draw(Canvas& can) {
	SDL_Rect contentBounds;
	contentBounds.x = style->outlineSz;
	contentBounds.y = style->outlineSz;
	contentBounds.w = bounds.w - (style->outlineSz * 2);
	contentBounds.h = bounds.h - (style->outlineSz * 2);
	can.setColor(style->colOutline);
	can.fillRect(0, 0, bounds.w, bounds.h);
	can.setRelViewport(contentBounds);
	can.setColor(select(*style, state));
	can.fillRect(0, 0, contentBounds.w, contentBounds.h);
	if (test()) {
		can.draw(tex, &texBounds);
		#if defined(DEBUG_TEXTBUTTON_DRAWTEXTURE) && DEBUG_TEXTBUTTON_DRAWTEXTURE
			auto oldColor = can.getColorState();
			can.setColor(DEBUG_TEXTBUTTON_DRAWTEXTURE_COLOR, getAlpha<DEBUG_TEXTBUTTON_DRAWTEXTURE_ALPHA>());
			can.fillRect(texBounds);
			can.setColorState(oldColor);
		#endif
	}
	can.clearViewport();
}


bool TextButton::test() const {
	return (tex != nullptr);
}


// get the preferred size of widget given text dimensions
IntPair TextButton::prefSizeOf(const IntPair& p) const {
	return calcPrefSize(p, style->padTextH, style->padTextV);
}


const Color& TextButton::select(const Style& bs, const WidgetState ws) {
	switch (ws) {
	case WidgetState::OUT:
		return bs.colBgOut;
	case WidgetState::OVER:
		return bs.colBgOver;
	case WidgetState::DOWN:
		return bs.colBgDown;
	}
	assert(false);
	return bs.colBgOut;
}


TextButton2::TextButton2() {
	sizePolicy = WidgetSizePolicy::PREFER;
}


TextButton2::~TextButton2() {
	SDL::freeNull(texOut);
	SDL::freeNull(texOver);
	SDL::freeNull(texDown);
}


void TextButton2::setStyle(const std::shared_ptr<Style>& s) {
	style = s;
	if (test())
		_doResize(prefSizeOf(IntPair{texBounds.w, texBounds.h}));
}


void TextButton2::setText(const std::string& str) {
	assert(style);
	text = str;
	IntPair prefSz;
	if (test()) {
		SDL::free(texOut);
		SDL::free(texOver);
		SDL::free(texDown);
	}
	style->tr->setColor(style->colTextOut);
	SDL_Surface* surf = style->tr->render(text);
	texBounds.w = surf->w;
	texBounds.h = surf->h;
	SDL_Texture* nTex = SDL::toTexture(surf);
	SDL::setAlpha(nTex, style->alphaOut);
	prefSz = prefSizeOf(IntPair{texBounds.w, texBounds.h});
	texOut = nTex;
	style->tr->setColor(style->colTextOut);
	surf = style->tr->render(text);
	nTex = SDL::toTexture(surf);
	SDL::setAlpha(nTex, style->alphaOver);
	texOver = nTex;
	style->tr->setColor(style->colTextDown);
	surf = style->tr->render(text);
	nTex = SDL::toTexture(surf);
	SDL::setAlpha(nTex, style->alphaDown);
	texDown = nTex;
	if (test()) {
		if (getSize() != prefSz)
			_doResize(prefSz);
	}
	else {
		_doResize(prefSz);
	}
}


void TextButton2::draw(Canvas& can) {
	SDL_Rect contentBounds;
	contentBounds.x = style->outlineSz;
	contentBounds.y = style->outlineSz;
	contentBounds.w = bounds.w - (style->outlineSz * 2);
	contentBounds.h = bounds.h - (style->outlineSz * 2);
	if (style->outlineSz != 0) {	// draw outline
		switch (state) {
		case WidgetState::OUT:
			can.setColor(style->colOutline, style->alphaOut);
			break;
		case WidgetState::OVER:
			can.setColor(style->colOutline, style->alphaOver);
			break;
		case WidgetState::DOWN:
			can.setColor(style->colOutline, style->alphaDown);
			break;
		}
		can.draw(SDL_Rect{0, 0, bounds.w, bounds.h}, style->outlineSz);
	}
	can.setRelViewport(contentBounds);
	switch (state) {
	case WidgetState::OUT:
		can.setColor(style->colBgOut);
		break;
	case WidgetState::OVER:
		can.setColor(style->colBgOver);
		break;
	case WidgetState::DOWN:
		can.setColor(style->colBgDown);
		break;
	}
	can.fillRect(0, 0, contentBounds.w, contentBounds.h);
	if (test()) {
		SDL_Texture* texture;
		switch (state) {
		case WidgetState::OUT:
			texture = texOut;
			break;
		case WidgetState::OVER:
			texture = texOver;
			break;
		case WidgetState::DOWN:
			texture = texDown;
			break;
		default:
			assert(false);
			texture = nullptr;	// remove uninitialized warning...
		}
		can.draw(texture, &texBounds);
		#if defined(DEBUG_TEXTBUTTON_DRAWTEXTURE) && DEBUG_TEXTBUTTON_DRAWTEXTURE
			auto oldColor = can.getColorState();
			can.setColor(DEBUG_TEXTBUTTON_DRAWTEXTURE_COLOR, getAlpha<DEBUG_TEXTBUTTON_DRAWTEXTURE_ALPHA>());
			can.fillRect(texBounds);
			can.setColorState(oldColor);
		#endif
	}
	can.clearViewport();
}


bool TextButton2::test() const {
	return (texOut != nullptr);
}


IntPair TextButton2::prefSizeOf(const IntPair& p) const {
	return calcPrefSize(p, style->padTextH, style->padTextV);
}


BitmapButton::BitmapButton() : texDim(-1, -1) {
	// texDim{-1, -1} means no texture has been set
	sizePolicy = WidgetSizePolicy::FIXED;
}



BitmapButton::~BitmapButton() {
	SDL::freeNull(texOut);
	SDL::freeNull(texOver);
	SDL::freeNull(texDown);
}


// cannot replace texture for a state once set
void BitmapButton::setTexture(const WidgetState s, SDL_Texture* tex) {
	SDL_Texture* const curTex = getTex(s);
	if (curTex != nullptr)
		logAndExit(RuntimeError{"BitmapButton::setTexture", "not allowed to overwrite texture"});
	IntPair dim;
	SDL::getDim(tex, dim.first, dim.second);
	if (texDim.first != -1) {	// texDim has been set
		if (dim != texDim)
			logAndExit(RuntimeError{"BitmapButton::setTexture", "texture dimension mismatch"});
	}
	else {
		texDim = dim;
		// resize to texture dimensions
		_doResize(texDim);
	}
	getTex(s) = tex;
}


void BitmapButton::draw(Canvas& can) {
	const int x = bounds.x;
	bounds.x = 0;
	const int y = bounds.y;
	bounds.y = 0;
	can.draw(getTex(state), &bounds);
	bounds.x = x;
	bounds.y = y;
}


IntPair BitmapButton::getPrefSize() const {
	if (texDim.first != -1)
		return texDim;
	else
		return IntPair{50, 50};
}


IntPair BitmapButton::getMinSize() const {
	if (texDim.first != -1)
		return texDim;
	else
		return IntPair{1, 1};	// arbitrary
}


void BitmapButton::_resize(const IntPair& p, const WidgetResizeFlag) {
	bounds.w = p.first;
	bounds.h = p.second;
}


SDL_Texture*& BitmapButton::getTex(const WidgetState s) {
	switch (s) {
	case WidgetState::OUT:
		return texOut;
	case WidgetState::OVER:
		return texOver;
	case WidgetState::DOWN:
		return texDown;
	}
	assert(false);
	// return some value to get rid of warning
	return texOut;
}
