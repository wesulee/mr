#include "sprite.h"


Sprite::Sprite(SDL_Texture* t, const SDL_Rect& r) : tex(t), clip(r) {
}


Sprite::Sprite(const Sprite& s): tex(s.tex), clip(s.clip) {
}


Sprite::~Sprite() {
	// do nothing
}


int Sprite::width() const {
	return clip.w;
}


int Sprite::height() const {
	return clip.h;
}


Sprite& Sprite::operator=(const Sprite& that) {
	tex = that.tex;
	clip = that.clip;
	return *this;
}


SDL_Texture* Sprite::getTexture() {
	return tex;
}


SDL_Rect* Sprite::getTextureBounds() {
	return &clip;
}


int Sprite::getDrawWidth() const {
	return clip.w;
}


int Sprite::getDrawHeight() const {
	return clip.h;
}
