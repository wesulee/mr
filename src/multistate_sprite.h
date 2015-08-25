#pragma once

#include "drawable.h"
#include "sprite.h"
#include <vector>


// like Sprite, but represents entity with multiple states (Sprites)
class MultistateSprite : public Drawable {
public:
	MultistateSprite() = default;
	virtual ~MultistateSprite() = default;
	void addState(const Sprite&);
	void setState(const std::size_t);
	std::size_t getIndex(void) const;
	// Drawable implementation
	SDL_Texture* getTexture(void);
	SDL_Rect* getTextureBounds(void);
	int getDrawWidth(void) const;
	int getDrawHeight(void) const;
private:
	std::vector<Sprite> states;
	std::size_t index = 0;
};
