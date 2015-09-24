#pragma once

#include "drawable.h"
#include "sdl_helper.h"


class Canvas;


// Represents a sprite from a SpriteSheet.
// Invalid once SpriteSheet is destroyed.
class Sprite : public Drawable {
public:
	Sprite() = default;
	Sprite(SDL_Surface*, SDL_Texture*, const SDL_Rect&);
	Sprite(const Sprite&);
	~Sprite() {}
	Sprite& operator=(const Sprite&);
	void draw(Canvas&, const int, const int);
	bool blit(SDL_Surface*, SDL_Rect*);
	// Drawable implementation
	SDL_Texture* getTexture(void);
	SDL_Rect* getTextureBounds(void);
	int getDrawWidth(void) const;
	int getDrawHeight(void) const;
private:
	SDL_Rect clip = {0, 0, 0, 0};
	SDL_Surface* surf = nullptr;
	SDL_Texture* tex = nullptr;
};
