#include "sprite_sheet.h"
#include "exception.h"
#include "logger.h"
#include "sprite.h"
#include <cassert>


Sprite SpriteSheet::get(const std::string& name) const {
	auto it = sprites.find(name);
	assert(it != sprites.end());
	if (it == sprites.end())
		Logger::instance().exit(RuntimeError{"SpriteSheet::get", "invalid name: " + name});
	return Sprite{surf, tex, it->second};
}


const SDL_Rect& SpriteSheet::getBounds(const std::string& name) const {
	auto find = sprites.find(name);
	if (find == sprites.end())
		Logger::instance().exit(RuntimeError{"SpriteSheet::getBounds", "invalid name: " + name});
	return find->second;
}
