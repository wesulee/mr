#pragma once

#include "drawable.h"
#include "sdl_helper.h"
#include <cassert>


// Represents an image that can be drawn to a canvas
class Image : public Drawable {
	Image(const Image&) = delete;
	void operator=(const Image&) = delete;
public:
	Image() = default;
	Image(SDL_Surface*);	// does not take ownership of surface
	Image(SDL_Texture*);	// takes ownership of texture
	~Image();

	void setWidth(const int);
	void setHeight(const int);
	void setAlpha(const Uint8);
	// Drawable methods
	SDL_Texture* getTexture() {return tex;}
	SDL_Rect* getTextureBounds() {return nullptr;}
	int getDrawWidth() const {return width;}
	int getDrawHeight() const {return height;}
private:
	SDL_Texture* tex = nullptr;
	int width = 0;
	int height = 0;
};


inline
void Image::setWidth(const int w) {
	assert(w >= 0);
	width = w;
}


inline
void Image::setHeight(const int h) {
	assert(h >= 0);
	height = h;
}


inline
void Image::setAlpha(const Uint8 a) {
	SDL_SetTextureAlphaMod(tex, a);
}
