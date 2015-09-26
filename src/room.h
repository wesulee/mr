#pragma once

#include "constants.h"
#include "json_reader.h"
#include "room_inc.h"
#include "room_qtree.h"
#include "sdl_helper.h"
#include "shapes.h"
#include <utility>
#include <vector>


class GameEntity;
class SpriteSheet;


struct RoomConnSpriteData {
	SpriteSheet* ss;
	std::pair<int, int> szNS;
	std::pair<int, int> szWE;
};


struct RoomConnection {
	std::pair<int, int> pos;
	SDL_Texture* tex = nullptr;
};


class RoomConnections {
	RoomConnections(const RoomConnections&) = delete;
	void operator=(const RoomConnections&) = delete;
public:
	RoomConnections() = default;
	~RoomConnections();
	void draw(Canvas&);
	void set(rapidjson::Document&, RoomConnSpriteData*);
	void render(void);
private:
	void drawNS(Canvas&, std::vector<RoomConnection>&, const int, const int);
	void drawWE(Canvas&, std::vector<RoomConnection>&, const int, const int);

	std::vector<RoomConnection> conn[4];
	RoomConnSpriteData* sprData;
};


struct RoomStruct {
	~RoomStruct();

	RoomConnections connections;
	QuadtreeRoom block;
	SDL_Surface* bgSurf = nullptr;
	SDL_Texture* bgTex = nullptr;
};


class Room {
	Room(const Room&) = delete;
	void operator=(const Room&) = delete;
public:
	Room();
	~Room();
	void draw(Canvas&);
	void set(rapidjson::Document&);
	bool space(const int, const int, const int, const int) const;
	void updateEntity(GameEntity&, const int, const int) const;
	void notifyClear(void);	// room has been cleared
private:
	SDL_Surface* renderBg(const rapidjson::Value&);

	RoomStruct* room = nullptr;
	RoomConnSpriteData sprData;
	SDL_Rect drawRect;
	mutable Rectangle test;
	bool cleared = false;
};
