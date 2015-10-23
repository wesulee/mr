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


void UniformAnimatedSpriteSource::setDuration(const Constants::float_type time) {
	frameDur = time;
}


// (x, y) are top left of rect of sprite in the provided texture
void UniformAnimatedSpriteSource::add(const int x, const int y) {
	frames.push_back(std::make_pair(x, y));
}


// frameTimeRem is the remaining time of the current frame
void UniformAnimatedSpriteSource::update(const Constants::float_type dt, Constants::float_type& frameTimeRem,
std::size_t& index) const {
	if (dt > frameTimeRem) {
		std::size_t indexDelta = (static_cast<std::size_t>((dt - frameTimeRem) / frameDur) + 1);
		index = ((index + indexDelta) % frames.size());
		frameTimeRem = (
			(frameDur * static_cast<Constants::float_type>(indexDelta))
			- (dt - frameTimeRem)
		);
	}
	else {
		frameTimeRem -= dt;
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


void UniformAnimatedSprite::update(const Constants::float_type dt) {
	src->update(dt, frameTimeRem, index);
}


void UniformAnimatedSprite::reset() {
	index = 0;
	frameTimeRem = 0;
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
