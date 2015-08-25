#include "room.h"
#include "canvas.h"
#include "constants.h"
#include "entity.h"
#include "game_data.h"
#include "json_data.h"
#include "logger.h"
#include "resource_manager.h"
#include "sprite_sheet.h"
#include "shapes.h"
#include <cassert>
#include <stdexcept>


class RoomWorkDraw;
static bool drawConnTickNS(RoomWorkDraw&);
static bool drawConnTickWE(RoomWorkDraw&);


class RoomWorkDraw : public RoomWork {
public:
	RoomWorkDraw(const Side, const std::vector<std::pair<int, int>>*, SpriteSheet*);
	~RoomWorkDraw() {/* do nothing */}
	bool tick(void) override;
	void finish(void) override;
	static void setSurfaces(SDL_Surface*, SDL_Surface*);

	static SDL_Surface* srcSurf;
	static SDL_Surface* dstSurf;
	SDL_Rect srcRect;
	SDL_Rect dstRect;
	bool (*doTick)(RoomWorkDraw&);
	const std::vector<std::pair<int, int>>* pairs;
	std::size_t index = 0;
	int drawLeft;
	float cur;
	float increment;
	bool inPair = false;
};


SDL_Surface* RoomWorkDraw::srcSurf = nullptr;
SDL_Surface* RoomWorkDraw::dstSurf = nullptr;


//! TODO incomplete implementation
RoomWorkDraw::RoomWorkDraw(const Side s, const std::vector<std::pair<int, int>>* p, SpriteSheet* ss)
: pairs(p) {
	// Note: these positions are relative to the surface, not screen
	switch (s) {
	case Side::NORTH:
		doTick = drawConnTickNS;
		srcRect = ss->getBounds("nr_n");
		dstRect.y = 0;
		break;
	case Side::SOUTH:
		doTick = drawConnTickNS;
		srcRect = ss->getBounds("nr_s");
		dstRect.y = Constants::roomHeight - 1 - srcRect.h;
		break;
	case Side::WEST:
		doTick = drawConnTickWE;
		srcRect = ss->getBounds("nr_w");
		dstRect.x = 0;
		break;
	case Side::EAST:
		doTick = drawConnTickWE;
		srcRect = ss->getBounds("nr_e");
		dstRect.x = Constants::roomWidth - 1 - srcRect.w;
		break;
	}
	dstRect.w = srcRect.w;
	dstRect.h = srcRect.h;
}


bool RoomWorkDraw::tick() {
	return doTick(*this);
}


void RoomWorkDraw::finish() {
	while (!tick())
		;
}


void RoomWorkDraw::setSurfaces(SDL_Surface* src, SDL_Surface* dst) {
	srcSurf = src;
	dstSurf = dst;
}


Room::Room() {
	drawRect.x = Constants::RoomX;
	drawRect.y = Constants::RoomY;
	drawRect.w = Constants::roomWidth;
	drawRect.h = Constants::roomHeight;
	block.setBounds(
		// block is 1 pixel longer on each side to add 4 block rects
		// to surround drawRect
		Rectangle{
			drawRect.x - 1,
			drawRect.y - 1,
			drawRect.w + 2,
			drawRect.h + 2
		}
	);
}


Room::~Room() {
	SDL::freeNull(bgSurf);
	SDL::freeNull(bgTex);
}


void Room::draw(Canvas& can) {
	can.draw(bgTex, &drawRect);
	#if defined(DEBUG_ROOM_BLOCK) && DEBUG_ROOM_BLOCK
		block.draw(can);
	#endif
}


void Room::set(RoomData& rd) {
	for (std::size_t i = 0; i < 4; ++i)
		connecting[i].clear();
	// add rectangles around edges of block
	const auto& bounds = block.getBounds();
	test.resize(bounds.width(), 1);
	test.move(bounds.getX(), bounds.getY());
	block.insert(test);	// top
	test.move(test.getX(), test.getY() + bounds.height() - 1);
	block.insert(test);	// bottom
	test.resize(1, bounds.height());
	test.move(bounds.getX(), bounds.getY());
	block.insert(test);	// left
	test.move(test.getX() + bounds.width() - 1 , test.getY());
	block.insert(test);	// right
	// add room
	for (const auto& r : rd.block) {
		test.resize(r.w, r.h);
		test.move(Constants::RoomX + r.x, Constants::RoomY + r.y);
		block.insert(test);
	}
	// set background image
	SDL::freeNull(bgSurf);
	SDL::freeNull(bgTex);
	bgSurf = GameData::instance().resources->generateSurface(rd);
	bgTex = SDL::newTexture(bgSurf);
	SpriteSheet* ss = GameData::instance().resources->getSpriteSheet("default");
	RoomWorkDraw::setSurfaces(GameData::instance().resources->getDefaultSurface(), bgSurf);
	for (const auto s : {Side::NORTH, Side::SOUTH, Side::WEST, Side::EAST}) {
		connecting[SideToIndex(s)].swap(rd.connecting[SideToIndex(s)]);
		if (!connecting[SideToIndex(s)].empty())
			worker.add(new RoomWorkDraw{s, connecting + SideToIndex(s), ss});
	}
	assert(!worker.empty());	// no connecting rooms
}


bool Room::space(const int x, const int y, const int w, const int h) const {
	test.set(x, y, w, h);
	return !block.collides(test);
}


// attempt to move entity to (x, y)
void Room::updateEntity(GameEntity& entity, const int x, const int y) const {
	SDL_Rect rect = entity.getBounds();
	int currentX = rect.x;
	int currentY = rect.y;
	int deltaX = x - rect.x;
	int deltaY = y - rect.y;
	int increment;
	if (deltaX != 0) {
		// attempt to move along x axis
		increment = deltaX > 0 ? 1 : -1;
		while ((deltaX != 0) && space(currentX + increment, currentY, rect.w, rect.h)) {
			currentX += increment;
			deltaX -= increment;
		}
	}
	if (deltaY != 0) {
		// move along y axis
		increment = deltaY > 0 ? 1 : -1;
		while ((deltaY != 0) && space(currentX, currentY + increment, rect.w, rect.h)) {
			currentY += increment;
			deltaY -= increment;
		}
	}
	entity.setPos(currentX, currentY);
}


void Room::notifyClear() {
	// finish remaining work now
	if (!worker.empty())
		worker.finish();
	// update background
	SDL::free(bgTex);
	bgTex = SDL::newTexture(bgSurf);
}


static bool drawConnTickNS(RoomWorkDraw& w) {
	if (w.inPair) {	// draw
		w.dstRect.x = static_cast<int>(w.cur);
		if (SDL_BlitSurface(w.srcSurf, &(w.srcRect), w.dstSurf, &(w.dstRect)) != 0) {
			SDL::logError("drawConnTickNS SDL_BlitSurface");
		}
		w.cur += w.increment;
		--(w.drawLeft);
		if (w.drawLeft == 0)
			w.inPair = false;
	}
	else {	// setup
		if (w.index == w.pairs->size())
			return true;
		const int width = (*w.pairs)[w.index].second - (*w.pairs)[w.index].first;
		w.drawLeft = width / w.srcRect.w;
		if (w.drawLeft < 2)
			throw std::runtime_error("drawConnTickNS drawLeft low");
		// draw rects along edges and then evenly space between
		w.cur = (*w.pairs)[w.index].first;
		const float avgPadding = static_cast<float>(width - (w.drawLeft * w.srcRect.w)) / (w.drawLeft - 1);
		w.increment = avgPadding + w.srcRect.w;
		w.inPair = true;
		++(w.index);
	}
	return false;
}


static bool drawConnTickWE(RoomWorkDraw& w) {
	if (w.inPair) {	// draw
		w.dstRect.y = static_cast<int>(w.cur);
		if (SDL_BlitSurface(w.srcSurf, &(w.srcRect), w.dstSurf, &(w.dstRect)) != 0) {
			SDL::logError("drawConnTickNS SDL_BlitSurface");
		}
		w.cur += w.increment;
		--(w.drawLeft);
		if (w.drawLeft == 0)
			w.inPair = false;
	}
	else {	// setup
		if (w.index == w.pairs->size())
			return true;
		const int height = (*w.pairs)[w.index].second - (*w.pairs)[w.index].first;
		w.drawLeft = height / w.srcRect.h;
		if (w.drawLeft < 2)
			throw std::runtime_error("drawConnTickWE drawLeft low");
		// draw rects along edges and then evenly space between
		w.cur = (*w.pairs)[w.index].first;
		const float avgPadding = static_cast<float>(height - (w.drawLeft * w.srcRect.h)) / (w.drawLeft - 1);
		w.increment = avgPadding + w.srcRect.h;
		w.inPair = true;
		++(w.index);
	}
	return false;
}
