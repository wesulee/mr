#include "sprite.h"
#include "canvas.h"
#include <cassert>


Sprite::Sprite(SDL_Surface* s, SDL_Texture* t, const SDL_Rect& r) : clip(r), surf(s), tex(t) {
}


Sprite::Sprite(const Sprite& o): clip(o.clip), surf(o.surf), tex(o.tex) {
}


Sprite& Sprite::operator=(const Sprite& that) {
	clip = that.clip;
	surf = that.surf;
	tex = that.tex;
	return *this;
}


void Sprite::draw(Canvas& can, const int x, const int y) {
	SDL_Rect r{x, y, clip.w, clip.h};
	can.draw(tex, &r);
}


bool Sprite::blit(SDL_Surface* dst, SDL_Rect* dstRect) {
	assert(surf != nullptr);
	assert(dst != nullptr);
	return (SDL_BlitSurface(surf, &clip, dst, dstRect) != 0);
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
