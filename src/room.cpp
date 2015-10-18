#include "room.h"
#include "canvas.h"
#include "constants.h"
#include "entity.h"
#include "game_data.h"
#include "logger.h"
#include "resource_manager.h"
#include "sprite.h"
#include "sprite_sheet.h"
#include <cassert>
#include <cmath>


namespace RoomConnHelper {
	constexpr char sprNN[] = "nr_n";
	constexpr char sprNS[] = "nr_s";
	constexpr char sprNW[] = "nr_w";
	constexpr char sprNE[] = "nr_e";

	template<class T>	// T is array
	void procRoomConnecting(const T& array, std::vector<RoomConnection>& vec) {
		namespace rj = rapidjson;
		vec.reserve(array.Size() / 2);
		// process one pair at a time
		RoomConnection tmpRC;
		for (rj::Value::ConstValueIterator it = array.Begin(); it != array.End();) {
			tmpRC.pos.first = JSONHelper::getIntAndInc(it);
			tmpRC.pos.second = JSONHelper::getIntAndInc(it);
			vec.push_back(tmpRC);
		}
	}
}


namespace RoomHelper {

// default SpriteSheet name
constexpr char defSSName[] = "default";


static void renderBgRepeatHoriz(Sprite& spr, SDL_Surface* dst, SDL_Rect& dstRect, const int count) {
	assert(count > 0);
	for (int i = 0; i < count; ++i, dstRect.x += dstRect.w)
		spr.blit(dst, &dstRect);
}


static void renderBgRepeatVert(Sprite& spr, SDL_Surface* dst, SDL_Rect& dstRect, const int count) {
	assert(count > 0);
	for (int i = 0; i < count; ++i, dstRect.y += dstRect.h)
		spr.blit(dst, &dstRect);
}


static void renderBgRepeat(Sprite& spr, SDL_Surface* dst, SDL_Rect& dstRect, int rx, int ry) {
	assert(dst != nullptr);
	assert(!((rx == 0) && (ry == 0)));
	assert(dstRect.w == spr.getDrawWidth());
	assert(dstRect.h == spr.getDrawHeight());
	switch (rx) {
	case -1:
		// ceil((remaining horiz space) / (sprite width))
		rx = ((dst->w - dstRect.x + dstRect.w - 1) / dstRect.w);
		break;
	case 0:
		break;
	default:
		// rx is repeat x, or the number of times to repeat drawing of sprite horizontally,
		//   so the number of rects to draw is rx+1
		++rx;
	}
	switch (ry) {
	case -1:
		// ceil((remaining vert space) / (sprite height))
		ry = ((dst->w - dstRect.y + dstRect.h - 1) / dstRect.h);
		break;
	case 0:
		break;
	default:
		++ry;
	}
	if (rx == 0) {
		renderBgRepeatVert(spr, dst, dstRect, ry);
	}
	else if (ry == 0) {
		renderBgRepeatHoriz(spr, dst, dstRect, rx);
	}
	else {
		const int startX = dstRect.x;
		for (int i = 0; i < ry; ++i, dstRect.x = startX, dstRect.y += dstRect.h)
			renderBgRepeatHoriz(spr, dst, dstRect, rx);
	}
}


inline
static int sign(const int n) {
	assert(n != 0);
	return (n > 0 ? 1 : -1);
}

} // namespace RoomHelper


RoomConnections::~RoomConnections() {
	// don't delete sprData
	for (std::size_t i = 0; i < 4; ++i) {
		for (auto& p : conn[i]) {
			SDL::freeNull(p.tex);
		}
	}
}


// Note: north/south grow horiz, west/east grow vert
void RoomConnections::draw(Canvas& can) {
	for (std::size_t i = 0; i < 4; ++i) {
		switch (static_cast<Side>(i)) {
		case Side::NORTH:
			drawNS(can, conn[i], 0, 0);
			break;
		case Side::SOUTH:
			drawNS(can, conn[i], 0, Constants::roomHeight - sprData->szNS.second);
			break;
		case Side::WEST:
			drawWE(can, conn[i], 0, 0);
			break;
		case Side::EAST:
			drawWE(can, conn[i], (Constants::roomWidth - sprData->szWE.first), 0);
			break;
		}
	}
}


void RoomConnections::set(rapidjson::Document& data, RoomConnSpriteData* sd) {
	sprData = sd;
	const rapidjson::Value& connections = data["conn"];
	RoomConnHelper::procRoomConnecting(connections["n"], conn[SideToIndex(Side::NORTH)]);
	RoomConnHelper::procRoomConnecting(connections["e"], conn[SideToIndex(Side::EAST)]);
	RoomConnHelper::procRoomConnecting(connections["s"], conn[SideToIndex(Side::SOUTH)]);
	RoomConnHelper::procRoomConnecting(connections["w"], conn[SideToIndex(Side::WEST)]);
}


// Generate textures
void RoomConnections::render() {
	//! TODO draw cleared when needed
	SDL_Rect dst;
	SDL_Surface* surf;
	Sprite spr;
	bool isNS;
	for (std::size_t i = 0; i < 4; ++i) {
		switch (static_cast<Side>(i)) {
		case Side::NORTH:
			spr = sprData->ss->get(RoomConnHelper::sprNN);
			isNS = true;
			break;
		case Side::SOUTH:
			spr = sprData->ss->get(RoomConnHelper::sprNS);
			isNS = true;
			break;
		case Side::WEST:
			spr = sprData->ss->get(RoomConnHelper::sprNW);
			isNS = false;
			break;
		case Side::EAST:
			spr = sprData->ss->get(RoomConnHelper::sprNE);
			isNS = false;
			break;
		}
		dst.x = 0;
		dst.y = 0;
		dst.w = spr.getDrawWidth();
		dst.h = spr.getDrawHeight();
		for (auto& p : conn[i]) {
			unsigned int count;
			if (isNS) {
				surf = SDL::newSurface32(p.pos.second - p.pos.first + 1, dst.h);
				count = static_cast<decltype(count)>(surf->w / dst.w);
				float cur = 0;
				float delta = (static_cast<float>(surf->w) / count);
				for (unsigned int j = 0; j < count; ++j) {
					if (j == (count-1)) {
						dst.x = (surf->w - dst.w);
						spr.blit(surf, &dst);
					}
					else {
						spr.blit(surf, &dst);
						cur += delta;
						dst.x = static_cast<int>(cur);
					}
				}
			}
			else {
				surf = SDL::newSurface32(dst.w, p.pos.second - p.pos.first + 1);
				count = static_cast<decltype(count)>(surf->h / dst.h);
				float cur = 0;
				float delta = (static_cast<float>(surf->h) / count);
				for (unsigned int j = 0; j < count; ++j) {
					if (j == (count-1)) {
						dst.y = (surf->h - dst.h);
						spr.blit(surf, &dst);
					}
					else {
						spr.blit(surf, &dst);
						cur += delta;
						dst.y = static_cast<int>(cur);
					}
				}
			}
			p.tex = SDL::toTexture(surf);
		}
	}
}


void RoomConnections::drawNS(Canvas& can, std::vector<RoomConnection>& vec, const int x, const int y) {
	SDL_Rect r;
	r.y = y;
	r.h = sprData->szNS.second;
	for (auto& p : vec) {
		r.x = (x + p.pos.first);
		r.w = (p.pos.second - p.pos.first + 1);
		can.draw(p.tex, &r);
	}
}


void RoomConnections::drawWE(Canvas& can, std::vector<RoomConnection>& vec, const int x, const int y) {
	SDL_Rect r;
	r.x = x;
	r.w = sprData->szWE.first;
	for (auto& p : vec) {
		r.y = (y + p.pos.first);
		r.h = (p.pos.second - p.pos.first + 1);
		can.draw(p.tex, &r);
	}
}


RoomStruct::~RoomStruct() {
	SDL::freeNull(bgSurf);
	SDL::freeNull(bgTex);
}


Room::Room() {
	sprData.ss = GameData::instance().resources->getSpriteSheet(RoomHelper::defSSName, true, false);
	Sprite sprConn = sprData.ss->get(RoomConnHelper::sprNN);
	sprData.szNS.first = sprConn.getDrawWidth();
	sprData.szNS.second = sprConn.getDrawHeight();
	sprConn = sprData.ss->get(RoomConnHelper::sprNW);
	sprData.szWE.first = sprConn.getDrawWidth();
	sprData.szWE.second = sprConn.getDrawHeight();
	drawRect.x = Constants::RoomX;
	drawRect.y = Constants::RoomY;
	drawRect.w = Constants::roomWidth;
	drawRect.h = Constants::roomHeight;
}


Room::~Room() {
	if (room != nullptr)
		delete room;
	GameData::instance().resources->freeSpriteSheet(RoomHelper::defSSName, true, false);
}


void Room::draw(Canvas& can) {
	assert(room != nullptr);
	can.draw(room->bgTex, &drawRect);
#if defined(DEBUG_ROOM_BLOCK) && DEBUG_ROOM_BLOCK
	room->block.draw(can);
#endif
	if (cleared) {
		can.setViewport(drawRect);
		room->connections.draw(can);
		can.clearViewport();
	}
}


void Room::set(rapidjson::Document& data) {
	namespace rj = rapidjson;
	assert(room == nullptr);
	// setup RoomStruct defaults
	room = new RoomStruct;
	room->block.setBounds(Rectangle{
		// block is 1 pixel longer on each side to add 4 block rects
		// to surround drawRect
		Constants::RoomX - 1,
		Constants::RoomY - 1,
		Constants::roomWidth + 2,
		Constants::roomHeight + 2
	});
	// add rectangles around edges of block
	const auto& bounds = room->block.getBounds();
	test.resize(bounds.width(), 1);
	test.move(bounds.getX(), bounds.getY());
	room->block.insert(test);	// top
	test.move(test.getX(), test.getY() + bounds.height() - 1);
	room->block.insert(test);	// bottom
	test.resize(1, bounds.height());
	test.move(bounds.getX(), bounds.getY());
	room->block.insert(test);	// left
	test.move(test.getX() + bounds.width() - 1, test.getY());
	room->block.insert(test);	// right
	{	// process block data
		SDL_Rect tmpRect;
		const rj::Value& block = data["block"];
		for (rj::Value::ConstValueIterator it = block.Begin(); it != block.End(); ++it) {
			rj::Value::ConstValueIterator it2 = it->Begin();
			JSONHelper::readRect(tmpRect, it2);
			test.resize(tmpRect.w, tmpRect.h);
			test.move(Constants::RoomX + tmpRect.x, Constants::RoomY + tmpRect.y);
			room->block.insert(test);
		}
	}
	// set background image
	room->bgSurf = renderBg(data["background"]);
	room->bgTex = SDL::newTexture(room->bgSurf);
	room->connections.set(data, &sprData);
}


bool Room::space(const int x, const int y, const int w, const int h) const {
	test.set(x, y, w, h);
	return !room->block.collides(test);
}


bool Room::space(const SDL_Rect& r) const {
	return !room->block.collides(r);
}


// Attempt to move entity by deltaPos, or as close to it as possible.
void Room::update(GameEntity& entity, const Vector2D<>& deltaPos) {
	SDL_Rect entityBounds = entity.getBounds();
	Vector2D<> entityPos = entity.getPos();
	Vector2D<> newPos{entityPos.x + deltaPos.x, entityPos.y + deltaPos.y};
	Vector2D<> actualNewPos;
	const IntPair deltaPosInt{
		static_cast<int>(newPos.x) - static_cast<int>(entityPos.x),
		static_cast<int>(newPos.y) - static_cast<int>(entityPos.y)
	};
	IntPair newDeltaPosInt{0, 0};	// default value
	if (deltaPosInt.first == 0) {
		if (deltaPosInt.second == 0) {
			// Since integer position has not changed, this position change is always allowed
			entity.setPos(newPos);
			return;
		}
		else {
			newDeltaPosInt.second = updateVert(entityBounds, deltaPosInt.second);
		}
	}
	else if (deltaPosInt.second == 0) {
		newDeltaPosInt.first = updateHoriz(entityBounds, deltaPosInt.first);
	}
	else if ((std::abs(deltaPosInt.first) > 1) || (std::abs(deltaPosInt.second) > 1)) {
		newDeltaPosInt = updateDiag(entityBounds, deltaPosInt);
		if (newDeltaPosInt.first == 0) {
			if (newDeltaPosInt.second == 0) {
				// Most likely entity walking into a block rect, attempt to move
				//   in line in each direction, since there may still be room.
				newDeltaPosInt.first = updateHoriz(entityBounds, deltaPosInt.first);
				newDeltaPosInt.second = updateVert(entityBounds, deltaPosInt.second);
			}
			else {
				// If one direction moves a little, and the other none, then
				//   most likely running into a block rect where updateDiag()
				//   returns after the first step. Continue trying to move
				//   in the direction in which step was taken.
				entityBounds.y += newDeltaPosInt.second;
				if ((deltaPosInt.second - newDeltaPosInt.second) != 0) {
					newDeltaPosInt.second += updateVert(entityBounds, deltaPosInt.second - newDeltaPosInt.second);
				}
			}
		}
		else if (newDeltaPosInt.second == 0) {
			entityBounds.x += newDeltaPosInt.first;
			if ((deltaPosInt.first - newDeltaPosInt.first) != 0) {
				newDeltaPosInt.first += updateHoriz(entityBounds, deltaPosInt.first - newDeltaPosInt.first);
			}
		}
	}
	else {
		newDeltaPosInt = updateStep(entityBounds, deltaPosInt);
	}
	if (deltaPosInt.first == newDeltaPosInt.first)
		actualNewPos.x = newPos.x;
	else
		actualNewPos.x = (entityPos.x + static_cast<decltype(entityPos.x)>(newDeltaPosInt.first));
	if (deltaPosInt.second == newDeltaPosInt.second)
		actualNewPos.y = newPos.y;
	else
		actualNewPos.y = (entityPos.y + static_cast<decltype(entityPos.y)>(newDeltaPosInt.second));
	entity.setPos(actualNewPos);
}


void Room::notifyClear() {
	room->connections.render();
	cleared = true;
}


// Render background image
SDL_Surface* Room::renderBg(const rapidjson::Value& data) {
	using namespace RoomHelper;
	namespace rj = rapidjson;
	SDL_Rect dstRect;
	SDL_Surface* surf = SDL::newSurface24(Constants::roomWidth, Constants::roomHeight);
	SDL_FillRect(surf, nullptr, SDL::mapRGB(surf->format, COLOR_BLACK));
	Sprite spr;
	int repeatX = 0;
	int repeatY = 0;
	for (rj::Value::ConstValueIterator it = data.Begin(); it != data.End(); ++it) {
		spr = sprData.ss->get((*it)["name"].GetString());
		dstRect.w = spr.getDrawWidth();
		dstRect.h = spr.getDrawHeight();
		dstRect.x = (*it)["x"].GetInt();
		dstRect.y = (*it)["y"].GetInt();
		if (it->HasMember("rx"))
			repeatX = (*it)["rx"].GetInt();
		if (it->HasMember("ry"))
			repeatY = (*it)["ry"].GetInt();
		if ((repeatX != 0) || (repeatY != 0)) {
			renderBgRepeat(spr, surf, dstRect, repeatX, repeatY);
			repeatX = 0;
			repeatY = 0;
		}
		else {
			spr.blit(surf, &dstRect);
		}
	}
	return surf;
}


IntPair Room::updateStep(const SDL_Rect& rect, const IntPair& delta) {
	assert(std::abs(delta.first) == 1);
	assert(std::abs(delta.second) == 1);
	SDL_Rect newRect = rect;
	const int signX = RoomHelper::sign(delta.first);
	const int signY = RoomHelper::sign(delta.second);
	IntPair newDelta{0, 0};
	newRect.x += signX;
	if (space(newRect)) {
		newDelta.first += signX;
		newRect.y += signY;
		if (space(newRect))
			newDelta.second += signY;
	}
	else {
		newRect.x = rect.x;	// reset
		newRect.y += signY;
		if (space(newRect)) {
			newDelta.second += signY;
			newRect.x += signX;
			if (space(newRect))
				newDelta.first += signX;
		}
	}
	return newDelta;
}


int Room::updateHoriz(const SDL_Rect& rect, const int delta) {
	assert(delta != 0);
	SDL_Rect newRect = rect;
	const int sign = RoomHelper::sign(delta);
	newRect.x += sign;
	const int countEnd = std::abs(delta);
	int count;
	for (count = 0; count < countEnd; ++count, newRect.x += sign) {
		if (!space(newRect))
			break;
	}
	return (count * sign);
}


int Room::updateVert(const SDL_Rect& rect, const int delta) {
	assert(delta != 0);
	SDL_Rect newRect = rect;
	const int sign = RoomHelper::sign(delta);
	newRect.y += sign;
	const int countEnd = std::abs(delta);
	int count;
	for (count = 0; count < countEnd; ++count, newRect.y += sign) {
		if (!space(newRect))
			break;
	}
	return (count * sign);
}


// use Bresenham's line algorithm with modification that diagonals
//   take additional step
//   (as in, going from (0,0) to (1,1) must also visit (0,1) or (1,0))
IntPair Room::updateDiag(const SDL_Rect& rect, const IntPair& delta) {
	assert(delta.first != 0);
	assert(delta.second != 0);
	if (std::abs(delta.first) >= std::abs(delta.second))
		return updateDiagX(rect, delta);
	else
		return updateDiagY(rect, delta);
}


IntPair Room::updateDiagX(const SDL_Rect& rect, const IntPair& delta) {
	SDL_Rect newRect = rect;
	IntPair newDelta{0, 0};
	const int countEnd = std::abs(delta.first);
	const int signX = RoomHelper::sign(delta.first);
	const int signY = RoomHelper::sign(delta.second);
	newRect.x += signX;
	int D = ((2 * delta.second * signY) - delta.first * signX);
	for (int count = 0; count < countEnd; ++count, newRect.x += signX) {
		if (D > 0) {
			if (space(newRect))
				newDelta.first += signX;
			else
				break;
			newRect.y += signY;
			if (space(newRect))
				newDelta.second += signY;
			else
				break;
			D += (2 * ((delta.second * signY) - (delta.first * signX)));
		}
		else {
			if (space(newRect))
				newDelta.first += signX;
			else
				break;
			D += (2 * delta.second * signY);
		}
	}
	return newDelta;
}


IntPair Room::updateDiagY(const SDL_Rect& rect, const IntPair& delta) {
	SDL_Rect newRect = rect;
	IntPair newDelta{0, 0};
	const int countEnd = std::abs(delta.second);
	const int signX = RoomHelper::sign(delta.first);
	const int signY = RoomHelper::sign(delta.second);
	newRect.y += signY;
	int D = ((2 * delta.first * signX) - delta.second * signY);
	for (int count = 0; count < countEnd; ++count, newRect.y += signY) {
		if (D > 0) {
			if (space(newRect))
				newDelta.second += signY;
			else
				break;
			newRect.x += signX;
			if (space(newRect))
				newDelta.first += signX;
			else
				break;
			D += (2 * ((delta.first * signX) - (delta.second * signY)));
		}
		else {
			if (space(newRect))
				newDelta.second += signY;
			else
				break;
			D += (2 * delta.first * signX);
		}
	}
	return newDelta;
}
