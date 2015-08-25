#include "animated_sprite.h"
#include <cassert>


UniformAnimatedSpriteSource::~UniformAnimatedSpriteSource() {
	// do nothing
}


void UniformAnimatedSpriteSource::setTexture(SDL_Texture* texture) {
	assert(texture != nullptr);
	assert(tex == nullptr);
	tex = texture;
}


void UniformAnimatedSpriteSource::setSize(const int w, const int h) {
	bounds.w = w;
	bounds.h = h;
}


// tickCount is number of ticks per frame
void UniformAnimatedSpriteSource::setTicks(const unsigned int ticks) {
	ticksMax = ticks;
}


// (x, y) are top left of rect of sprite in the provided texture
void UniformAnimatedSpriteSource::add(const int x, const int y) {
	frames.push_back(std::make_pair(x, y));
}


void UniformAnimatedSpriteSource::update(unsigned int& ticks, std::size_t& index) const {
	if (++ticks > ticksMax) {
		ticks = 0;
		if (++index >= frames.size())
			index = 0;
	}
}


SDL_Texture* UniformAnimatedSpriteSource::getTexture() {
	return tex;
}


SDL_Rect* UniformAnimatedSpriteSource::getTextureBounds(const std::size_t index) {
	bounds.x = frames[index].first;
	bounds.y = frames[index].second;
	return &bounds;
}


int UniformAnimatedSpriteSource::getDrawWidth() const {
	return bounds.w;
}


int UniformAnimatedSpriteSource::getDrawHeight() const {
	return bounds.h;
}


void UniformAnimatedSprite::update() {
	src->update(ticks, index);
}


void UniformAnimatedSprite::reset() {
	ticks = 0;
	index = 0;
}


void UniformAnimatedSprite::setSource(UniformAnimatedSpriteSource* s) {
	src = s;
}


SDL_Texture* UniformAnimatedSprite::getTexture() {
	return src->getTexture();
}


SDL_Rect* UniformAnimatedSprite::getTextureBounds() {
	return src->getTextureBounds(index);
}


int UniformAnimatedSprite::getDrawWidth() const {
	return src->getDrawWidth();
}


int UniformAnimatedSprite::getDrawHeight() const {
	return src->getDrawHeight();
}
