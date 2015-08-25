#pragma once

#include "color.h"
#include "creature_type.h"
#include "sdl_header.h"
#include <forward_list>
#include <map>
#include <string>
#include <unordered_map>
#include <vector>


struct RoomSpriteData {
	std::string name;	// sprite name
	int x = 0;
	int y = 0;
};


struct RoomCreatureData {
	std::string name;
	CreatureType type = CreatureType::NONE;	// default
	int x = 0;
	int y = 0;
};


struct RoomData {
	// indices for each side, pairs in increasing order
	std::vector<std::pair<int, int>> connecting[4];
	std::vector<RoomSpriteData> bg;		// background image data
	std::vector<SDL_Rect> block;		// block rectangles
	std::vector<RoomCreatureData> creatures;
};


struct SpriteSheetData {
	std::unordered_map<std::string, SDL_Rect> sprites;
	std::string image;
};


struct CreatureData {
	std::map<std::string, std::string> attributes;
	std::forward_list<std::map<std::string, std::string>> animations;
	std::vector<std::string> sheets;
};
