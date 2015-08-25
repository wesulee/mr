#pragma once

#include "drawable.h"
#include "sdl_helper.h"


// Represents a (relative position) static sprite from the loaded SpriteSheet.
class Sprite : public Drawable {
public:
	Sprite() = default;
	Sprite(SDL_Texture*, const SDL_Rect&);
	Sprite(const Sprite&);
	~Sprite();

	int width(void) const;
	int height(void) const;

	Sprite& operator=(const Sprite&);
	// Drawable implementation
	SDL_Texture* getTexture();
	SDL_Rect* getTextureBounds();
	int getDrawWidth() const;
	int getDrawHeight() const;
private:
	SDL_Texture* tex = nullptr;
	SDL_Rect clip = {0, 0, 0, 0};
};
