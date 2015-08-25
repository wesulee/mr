#pragma once

#include "sdl_header.h"
#include <string>
#include <unordered_map>


class Sprite;


// A Sprite is valid only during lifetime of the instance that created it.
class SpriteSheet {
public:
	SpriteSheet(SDL_Texture*, const std::string&);
	~SpriteSheet() {}
	void add(const std::string&, const SDL_Rect&);
	Sprite get(const std::string&) const;
	const SDL_Rect& getBounds(const std::string&) const;
	SDL_Texture* getTexture(void);	// should probably only be called by ResourceManager
	const std::string& getImageName(void) const;
private:
	std::unordered_map<std::string, SDL_Rect> sprites;
	std::string imgName;
	SDL_Texture* tex;
};


inline
SDL_Texture* SpriteSheet::getTexture() {
	return tex;
}


inline
const std::string& SpriteSheet::getImageName() const {
	return imgName;
}
