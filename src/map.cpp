#include "map.h"
#include "canvas.h"
#include "game_data.h"
#include "input_handler.h"
#include "save_data.h"
#include <algorithm>	// max
#include <cmath>		// abs


namespace MapSettings {
	constexpr int blockPad = 3;
	constexpr Color bgColor = COLOR_BLACK;
	constexpr Color curColor = COLOR_YELLOW;
	constexpr Color clearColor = COLOR_GREEN;
	constexpr Color unclearColor = COLOR_RED;
	// distance from x axis to have opaque alpha
	constexpr int distAlphaX = 100;
	constexpr int distAlphaY = 200;
	constexpr Uint8 minAlpha = 25;
}


static Uint8 calcAlpha(const int distAlpha, const int dist) {
	const int distAbs = std::abs(dist);
	if (distAbs >= distAlpha)
		return SDL_ALPHA_OPAQUE;
	// linear scale, offset by minAlpha
	return (
		MapSettings::minAlpha
		+ static_cast<Uint8>(
			(static_cast<float>(distAbs) / distAlpha) * (SDL_ALPHA_OPAQUE - MapSettings::minAlpha)
		)
	);
}


Map::Map() : tex(nullptr) {
	curX = curY = -1;
	drawX = drawY = -1;
	dstFill.w = Constants::MapBlockSz;
	dstFill.h = dstFill.w;
	// size of blocks + size of padding
	dst.w = (Constants::MapCountX * Constants::MapBlockSz) + ((Constants::MapCountX + 1) * MapSettings::blockPad);
	dst.h = (Constants::MapCountY * Constants::MapBlockSz) + ((Constants::MapCountY + 1) * MapSettings::blockPad);
	// occupy bottom right corner
	dst.x = Constants::RoomX + Constants::roomWidth - dst.w;
	dst.y = Constants::RoomY + Constants::roomHeight - dst.h;
	surf = SDL::newSurface32(dst.w, dst.h);
	// draw default
	Uint32 col = SDL::mapRGB(surf->format, MapSettings::bgColor);
	SDL_FillRect(surf, nullptr, col);
	col = SDL::mapRGB(surf->format, MapSettings::unclearColor);
	for (int x = 0; x < Constants::MapCountX; ++x) {
		for (int y = 0; y < Constants::MapCountY; ++y) {
			drawBlock(x, y, col);
		}
	}
	refresh();
}


Map::~Map() {
	SDL::free(surf);
	SDL::free(tex);
}


void Map::draw(Canvas& can) {
	can.draw(tex, &dst);
}


void Map::setCur(const int x, const int y) {
	assert((x >= 0) && (x < Constants::MapCountX));
	assert((y >= 0) && (y < Constants::MapCountY));
	if (drawX != -1) {
		drawBlock(drawX, drawY, SDL::mapRGB(surf->format, MapSettings::clearColor));
		drawX = drawY = -1;
	}
	if (vec.test(vecIndex(x, y))) {
		drawX = x;
		drawY = y;
	}
	curX = x;
	curY = y;
	drawBlock(x, y, SDL::mapRGB(surf->format, MapSettings::curColor));
}


void Map::setClear(const int x, const int y) {
	vec.set(vecIndex(x, y));
	// if setClear called on current room, do not draw yet
	if ((x == curX) && (y == curX)) {
		drawX = x;
		drawY = y;
	}
	else
		drawBlock(x, y, SDL::mapRGB(surf->format, MapSettings::clearColor));
}


// update texture after modifying surface
void Map::refresh() {
	SDL::freeNull(tex);
	tex = SDL::newTexture(surf);
	SDL_SetTextureAlphaMod(tex, alpha);
}


void Map::notifyMoved() {
	const int dx = GameData::instance().inputHandler->mX() - dst.x;
	const int dy = GameData::instance().inputHandler->mY() - dst.y;
	if (dx >= 0) {
		if (dy >= 0) {	// inside
			alpha = MapSettings::minAlpha;
		}
		else {			// above
			alpha = calcAlpha(MapSettings::distAlphaX, dy);
		}
	}
	else {
		if (dy >= 0) {	// left
			alpha = calcAlpha(MapSettings::distAlphaY, dx);
		}
		else {			// above, left
			alpha = std::max(
				calcAlpha(MapSettings::distAlphaX, dy),
				calcAlpha(MapSettings::distAlphaY, dx)
			);
		}
	}
	SDL_SetTextureAlphaMod(tex, alpha);
}


void Map::setSaveData(SaveData& data) const {
	data.mapVec = vec.toVector();
	data.roomX = curX;
	data.roomY = curY;
}


void Map::getSaveData(const SaveData& data) {
	vec.set(data.mapVec);
	for (int x = 0; x < Constants::MapCountX; ++x) {
		for (int y = 0; y < Constants::MapCountY; ++y) {
			if (vec.test(vecIndex(x, y)))
				setClear(x, y);
		}
	}
	setCur(data.roomX, data.roomY);
	refresh();
}


void Map::drawBlock(const int x, const int y, uint32_t col) {
	assert((x >= 0) && (x < Constants::MapCountX));
	assert((y >= 0) && (y < Constants::MapCountY));
	setFillRect(x, y);
	SDL_FillRect(surf, &dstFill, col);
}


void Map::setFillRect(const int x, const int y) {
	// set initial position of (0, 0)
	dstFill.x = MapSettings::blockPad;
	dstFill.y = dst.h - (MapSettings::blockPad + Constants::MapBlockSz);
	// add offsets
	dstFill.x += x * (Constants::MapBlockSz + MapSettings::blockPad);
	dstFill.y -= y * (Constants::MapBlockSz + MapSettings::blockPad);
}
