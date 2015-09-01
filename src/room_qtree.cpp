#include "room_qtree.h"
#include "constants.h"
#include <cassert>
#ifndef NDEBUG
#include "canvas.h"


void QuadtreeRoom::draw(Canvas& can) {
	if (nodes != nullptr) {
		for (std::size_t i = 0; i < QTREE_ROOM_END; ++i)
			nodes[i].draw(can);
	}
	else {
		auto oldColor = can.getColorState();
		can.setColor(DEBUG_ROOM_BLOCK_COLOR, getAlpha<DEBUG_ROOM_BLOCK_ALPHA>());
		for (const auto& r : objects)
			can.fillRect(r.getX(), r.getY(), r.width(), r.height());
#if DEBUG_ROOM_BOUNDS
		can.setColor(DEBUG_ROOM_BOUNDS_COLOR, getAlpha<DEBUG_ROOM_BOUNDS_ALPHA>());
		can.fillRect(		// top
			bounds.getX(),
			bounds.getY(),
			bounds.width(),
			DEBUG_ROOM_BOUNDS_SIZE
		);
		can.fillRect(		// bottom
			bounds.getX(),
			bounds.getY() + bounds.height() - DEBUG_ROOM_BOUNDS_SIZE,
			bounds.width(),
			DEBUG_ROOM_BOUNDS_SIZE
		);
		can.fillRect(		// left
			bounds.getX(),
			bounds.getY(),
			DEBUG_ROOM_BOUNDS_SIZE,
			bounds.height()
		);
		can.fillRect(		// right
			bounds.getX() + bounds.width() - DEBUG_ROOM_BOUNDS_SIZE,
			bounds.getY(),
			DEBUG_ROOM_BOUNDS_SIZE,
			bounds.height()
		);
#endif // DEBUG_ROOM_BOUNDS
		can.setColorState(oldColor);
	}
}

#endif // NDEBUG


void QuadtreeRoom::insert(const Rectangle& r) {
	if (nodes != nullptr) {
		for (std::size_t i = 0; i < QTREE_ROOM_END; ++i) {
			if (nodes[i].getBounds().intersects(r))
				nodes[i].insert(r);
		}
		return;
	}

	if (objects.size() < Constants::RoomQTreeNodeCap)
		insertObject(r);
	else {
		split();
		insert(r);
	}
}


// does the given rect collide with any other rects?
bool QuadtreeRoom::collides(const Rectangle& rect) const {
	if (!rect.intersects(bounds))
		return false;
	if (nodes != nullptr) {
		return (
			nodes[QTREE_ROOM_NW].collides(rect)
			|| nodes[QTREE_ROOM_NE].collides(rect)
			|| nodes[QTREE_ROOM_SW].collides(rect)
			|| nodes[QTREE_ROOM_SE].collides(rect)
		);
	}
	else {
		for (const auto& r : objects) {
			if (rect.intersects(r))
				return true;
		}
		return false;
	}
}


void QuadtreeRoom::clear() {
	std::vector<Rectangle> emptyObjects;
	objects.swap(emptyObjects);
	if (nodes != nullptr) {
		delete[] nodes;
		nodes = nullptr;
	}
}


void QuadtreeRoom::split() {
	assert(nodes == nullptr);
	Rectangle qbounds;
	nodes = new QuadtreeRoom[4];
	const int width = bounds.width();
	const int height = bounds.height();
	const int qwidth = width / 2;
	const int qheight = height / 2;
	// NW
	qbounds.set(bounds.getX(), bounds.getY(), qwidth, qheight);
	nodes[QTREE_ROOM_NW].setBounds(qbounds);
	// NE
	qbounds.set(bounds.getX() + qwidth, bounds.getY(), width - qwidth, qheight);
	nodes[QTREE_ROOM_NE].setBounds(qbounds);
	// SW
	qbounds.set(bounds.getX(), bounds.getY() + qheight, qwidth, height - qheight);
	nodes[QTREE_ROOM_SW].setBounds(qbounds);
	// SE
	qbounds.set(bounds.getX() + qwidth, bounds.getY() + qheight, width - qwidth, height - qheight);
	nodes[QTREE_ROOM_SE].setBounds(qbounds);
#ifndef NDEBUG
	// check split is correct
	const int newArea = (
		nodes[QTREE_ROOM_NW].getBounds().area()
		+ nodes[QTREE_ROOM_NE].getBounds().area()
		+ nodes[QTREE_ROOM_SW].getBounds().area()
		+ nodes[QTREE_ROOM_SE].getBounds().area()
	);
	assert(bounds.area() == newArea);
#endif // NDEBUG
	// move rects into new quads
	for (const auto& r : objects) {
		// no checking needed since new quad only inserts correct part of rect
		for (std::size_t i = 0; i < QTREE_ROOM_END; ++i)
			nodes[i].insert(r);
	}
	objects.clear();
	objects.shrink_to_fit();
}


void QuadtreeRoom::insertObject(const Rectangle& r) {
	// only insert part of rect that is inside bounds
	Rectangle nrect = bounds.intersection(r);
	if (!nrect.empty())
		objects.push_back(nrect);
}
