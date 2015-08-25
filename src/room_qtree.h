#pragma once

#include "shapes.h"
#include <vector>


class Canvas;


// A quadtree specifically for block rectangles
// Rather than leave rects that don't fit into a new quad in parent, split it so it does
class QuadtreeRoom {
	QuadtreeRoom(const QuadtreeRoom&) = delete;
	void operator=(const QuadtreeRoom&) = delete;
	enum Indices : unsigned int {QTREE_ROOM_NW = 0, QTREE_ROOM_NE, QTREE_ROOM_SW, QTREE_ROOM_SE, QTREE_ROOM_END};
public:
	QuadtreeRoom() = default;
	~QuadtreeRoom();
	void draw(Canvas&);		// for debug
	void setBounds(const Rectangle&);
	const Rectangle& getBounds(void) const;
	void insert(const Rectangle&);
	bool collides(const Rectangle&) const;
	void clear(void);
private:
	void split(void);
	void insertObject(const Rectangle&);

	std::vector<Rectangle> objects;
	Rectangle bounds;
	QuadtreeRoom* nodes = nullptr;
};


inline
QuadtreeRoom::~QuadtreeRoom() {
	if (nodes != nullptr)
		delete[] nodes;
}


inline
void QuadtreeRoom::setBounds(const Rectangle& r) {
	bounds = r;
}


inline
const Rectangle& QuadtreeRoom::getBounds() const {
	return bounds;
}
