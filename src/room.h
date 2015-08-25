#pragma once

#include "constants.h"
#include "room_inc.h"
#include "room_qtree.h"
#include "sdl_helper.h"
#include "shapes.h"
#include <forward_list>
#include <memory>
#include <utility>
#include <vector>


class GameEntity;
class RoomData;


class RoomWork {
public:
	RoomWork() {}
	virtual ~RoomWork() {}
	virtual bool tick(void) = 0;
	virtual void finish(void) = 0;
};


class RoomWorker {
	RoomWorker(const RoomWorker&) = delete;
	void operator=(const RoomWorker&) = delete;
public:
	RoomWorker() = default;
	~RoomWorker();
	bool empty(void) const;
	void add(RoomWork*);
	bool tick(void);
	void finish(void);
private:
	std::forward_list<RoomWork*> jobs;
};


class Room {
	Room(const Room&) = delete;
	void operator=(const Room&) = delete;
public:
	Room();
	~Room();
	void draw(Canvas&);
	void set(RoomData&);
	bool space(const int, const int, const int, const int) const;
	void updateEntity(GameEntity&, const int, const int) const;
	bool workTick(void);
	void notifyClear(void);	// room has been cleared
private:
	std::vector<std::pair<int, int>> connecting[4];
	QuadtreeRoom block;
	SDL_Rect drawRect;
	mutable Rectangle test;
	RoomWorker worker;
	SDL_Surface* bgSurf = nullptr;
	SDL_Texture* bgTex = nullptr;
};


inline
RoomWorker::~RoomWorker() {
	while (!jobs.empty()) {
		delete jobs.front();
		jobs.pop_front();
	}
}


inline
bool RoomWorker::empty() const {
	return jobs.empty();
}


inline
void RoomWorker::add(RoomWork* w) {
	jobs.push_front(w);
}


inline
bool RoomWorker::tick() {
	if (jobs.empty())
		return true;
	if (jobs.front()->tick()) {
		delete jobs.front();
		jobs.pop_front();
	}
	return false;
}


inline
void RoomWorker::finish() {
	while (!tick())
		;
}
