#include "multistate_sprite.h"
#include <cassert>


void MultistateSprite::addState(const Sprite& spr) {
	states.push_back(spr);
}


void MultistateSprite::setState(const std::size_t i) {
	assert(i < states.size());
	index = i;
}


std::size_t MultistateSprite::getIndex() const {
	return index;
}


SDL_Texture* MultistateSprite::getTexture() {
	return states[index].getTexture();
}


SDL_Rect* MultistateSprite::getTextureBounds() {
	return states[index].getTextureBounds();
}


int MultistateSprite::getDrawWidth() const {
	return states[index].getDrawWidth();
}


int MultistateSprite::getDrawHeight() const {
	return states[index].getDrawHeight();
}
