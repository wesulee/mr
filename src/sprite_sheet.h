#pragma once

#include "sdl_header.h"
#include <string>
#include <unordered_map>


class ResourceManager;
class Sprite;


class SpriteSheet {
	friend ResourceManager;
public:
	SpriteSheet() = default;
	~SpriteSheet() {}
	Sprite get(const std::string&) const;
	const SDL_Rect& getBounds(const std::string&) const;
	SDL_Surface* getSurface(void);
	SDL_Texture* getTexture(void);
	const std::string& getImageName(void) const;
private:
	std::unordered_map<std::string, SDL_Rect> sprites;
	std::string imgName;
	SDL_Surface* surf = nullptr;
	SDL_Texture* tex = nullptr;
};


inline
SDL_Surface* SpriteSheet::getSurface() {
	return surf;
}


inline
SDL_Texture* SpriteSheet::getTexture() {
	return tex;
}


inline
const std::string& SpriteSheet::getImageName() const {
	return imgName;
}
