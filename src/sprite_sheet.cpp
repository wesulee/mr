#include "sprite_sheet.h"
#include "exception.h"
#include "sprite.h"
#include <cassert>


SpriteSheet::SpriteSheet(SDL_Texture* texture, const std::string& name) : imgName(name), tex(texture) {
}


void SpriteSheet::add(const std::string& name, const SDL_Rect& rect) {
	#ifndef NDEBUG
	auto f = sprites.find(name);
	assert(f == sprites.end());
	#endif
	sprites[name] = rect;
}


Sprite SpriteSheet::get(const std::string& name) const {
	auto find = sprites.find(name);
	if (find == sprites.end())
		logAndExit(RuntimeError{"SpriteSheet::get", "invalid name: " + name});
	return Sprite{tex, find->second};
}


const SDL_Rect& SpriteSheet::getBounds(const std::string& name) const {
	auto find = sprites.find(name);
	if (find == sprites.end())
		logAndExit(RuntimeError{"SpriteSheet::getBounds", "invalid name: " + name});
	return find->second;
}
