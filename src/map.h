#pragma once

#include "bit_vector.h"
#include "constants.h"
#include "sdl_helper.h"
#include <cassert>
#include <cstdint>


class Canvas;
class SaveData;


// Usage: call setClear, then setCur on init
// Note: room (0, 0) is the bottom left
class Map {
	typedef BitVector<Constants::MapCountX * Constants::MapCountY> MapBitVector;
	Map(const Map&) = delete;
	void operator=(const Map&) = delete;
public:
	Map();
	~Map();
	void draw(Canvas&);
	void setCur(const int, const int);
	void setClear(const int, const int);
	void refresh(void);
	void notifyMoved(void);
	int getCurX(void) const;
	int getCurY(void) const;
	bool isCleared(const int, const int) const;
	void setSaveData(SaveData&) const;
	void getSaveData(const SaveData&);
private:
	void drawBlock(const int, const int, uint32_t);
	void setFillRect(const int, const int);
	std::size_t vecIndex(const int, const int) const;

	MapBitVector vec;
	SDL_Rect dst;
	SDL_Rect dstFill;	// fill rect for surface
	SDL_Surface* surf;
	SDL_Texture* tex;
	int curX;
	int curY;
	int drawX;
	int drawY;
	Uint8 alpha = SDL_ALPHA_OPAQUE;
};


inline
int Map::getCurX() const {
	return curX;
}


inline
int Map::getCurY() const {
	return curY;
}


inline
bool Map::isCleared(const int x, const int y) const {
	return vec.test(vecIndex(x, y));
}


inline
std::size_t Map::vecIndex(const int x, const int y) const {
	assert((x >= 0) && (x < Constants::MapCountX));
	assert((y >= 0) && (y < Constants::MapCountY));
	return static_cast<std::size_t>((x * Constants::MapCountY) + y);
}
