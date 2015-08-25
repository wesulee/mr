#pragma once

#include "sdl_header.h"
#include <algorithm>
#include <cassert>
#include <cstring>


// coords indices
#define RECTANGLE_X0 0
#define RECTANGLE_Y0 1
#define RECTANGLE_X1 2
#define RECTANGLE_Y1 3


class Rectangle {
public:
	Rectangle();
	Rectangle(const Rectangle&);
	Rectangle(const int, const int, const int, const int);	// x, y, w, h
	Rectangle(const SDL_Rect&);
	~Rectangle() = default;

	bool empty(void) const;
	int getX(void) const;
	int getY(void) const;
	int width(void) const;
	int height(void) const;
	int area(void) const;
	void resize(const int, const int);
	void set(const int, const int, const int, const int);
	void set2(const int, const int, const int, const int);
	void move(const int, const int);
	bool contains(const int, const int) const;
	bool intersects(const Rectangle&) const;
	Rectangle intersection(const Rectangle&) const;
	Rectangle& operator=(const Rectangle&);
private:
	int coords[4];
};


class Circle {
public:
	Circle() = default;
	Circle(const int, const int, const int);
	~Circle() = default;

	int x;
	int y;
	int r;
};


namespace Shape {
	bool contains(const SDL_Rect&, const int, const int);
	bool contains(const SDL_Rect&, const SDL_Rect&);
	bool intersects(const SDL_Rect&, const SDL_Rect&);
	bool intersects(const SDL_Rect&, const Circle&);
	bool intersects(const Circle&, const Circle&);
	SDL_Rect bounds(const Circle&);
}


inline
Rectangle::Rectangle() {
	std::memset(coords, 0, sizeof(coords));
}


inline
Rectangle::Rectangle(const Rectangle& r) {
	std::memcpy(coords, r.coords, sizeof(coords));
}


inline
Rectangle::Rectangle(const int x, const int y, const int w, const int h) {
	assert(w > 0);
	assert(h > 0);
	coords[RECTANGLE_X0] = x;
	coords[RECTANGLE_Y0] = y;
	coords[RECTANGLE_X1] = x + w - 1;
	coords[RECTANGLE_Y1] = y + h - 1;
}


inline
Rectangle::Rectangle(const SDL_Rect& r) : Rectangle(r.x, r.y, r.w, r.h) {
}


inline
bool Rectangle::empty() const {
	return (
		(coords[RECTANGLE_X0] == 0)
		&& (coords[RECTANGLE_Y0] == 0)
		&& (coords[RECTANGLE_X1] == 0)
		&& (coords[RECTANGLE_Y1] == 0)
	);
}


inline
int Rectangle::getX() const {
	return coords[RECTANGLE_X0];
}


inline
int Rectangle::getY() const {
	return coords[RECTANGLE_Y0];
}


inline
int Rectangle::width() const {
	return (coords[RECTANGLE_X1] - coords[RECTANGLE_X0] + 1);
}


inline
int Rectangle::height() const {
	return (coords[RECTANGLE_Y1] - coords[RECTANGLE_Y0] + 1);
}


inline
int Rectangle::area() const {
	return (width() * height());
}


// modify width and height of rectangle, leaving top-left coordinate same
inline
void Rectangle::resize(const int w, const int h) {
	assert(w > 0);
	assert(h > 0);
	coords[RECTANGLE_X1] = coords[RECTANGLE_X0] + w - 1;
	coords[RECTANGLE_Y1] = coords[RECTANGLE_Y0] + h - 1;
}


inline
void Rectangle::set(const int x, const int y, const int w, const int h) {
	assert(w > 0);
	assert(h > 0);
	coords[RECTANGLE_X0] = x;
	coords[RECTANGLE_Y0] = y;
	coords[RECTANGLE_X1] = x + w - 1;
	coords[RECTANGLE_Y1] = y + h - 1;
}


inline
void Rectangle::set2(const int x0, const int y0, const int x1, const int y1) {
	assert(x0 <= x1);
	assert(y0 <= y1);
	coords[RECTANGLE_X0] = x0;
	coords[RECTANGLE_Y0] = y0;
	coords[RECTANGLE_X1] = x1;
	coords[RECTANGLE_Y1] = y1;
}


// move top-left coordinate of rectangle
inline
void Rectangle::move(const int x, const int y) {
	set(x, y, width(), height());
}


// does Rectangle contain point?
inline
bool Rectangle::contains(const int x, const int y) const {
	return (
		(x >= coords[RECTANGLE_X0])
		&& (x <= coords[RECTANGLE_X1])
		&& (y >= coords[RECTANGLE_Y0])
		&& (y <= coords[RECTANGLE_Y1])
	);
}


inline
bool Rectangle::intersects(const Rectangle& r) const {
	return (
		(coords[RECTANGLE_X0] < r.coords[RECTANGLE_X1])
		&& (coords[RECTANGLE_X1] > r.coords[RECTANGLE_X0])
		&& (coords[RECTANGLE_Y0] < r.coords[RECTANGLE_Y1])
		&& (coords[RECTANGLE_Y1] > r.coords[RECTANGLE_Y0])
	);
}


inline
Rectangle Rectangle::intersection(const Rectangle& r) const {
	Rectangle ret;
	const int x0 = std::max(coords[RECTANGLE_X0], r.coords[RECTANGLE_X0]);
	const int x1 = std::min(coords[RECTANGLE_X1], r.coords[RECTANGLE_X1]);
	if (x0 <= x1) {
		const int y0 = std::max(coords[RECTANGLE_Y0], r.coords[RECTANGLE_Y0]);
		const int y1 = std::min(coords[RECTANGLE_Y1], r.coords[RECTANGLE_Y1]);
		if (y0 <= y1)
			ret.set2(x0, y0, x1, y1);
	}
	return ret;
}


inline
Rectangle& Rectangle::operator=(const Rectangle& r) {
	std::memcpy(coords, r.coords, sizeof(coords));
	return *this;
}


inline
Circle::Circle(const int _x, const int _y, const int _r) : x(_x), y(_y), r(_r) {
}


inline
bool Shape::contains(const SDL_Rect& r, const int x, const int y) {
	return (
		(x >= r.x)
		&& (x < (r.x + r.w))
		&& (y >= r.y)
		&& (y < (r.y + r.h))
	);
}


// is r2 fully contained in r1?
inline
bool Shape::contains(const SDL_Rect& r1, const SDL_Rect& r2) {
	return (
		(r2.x >= r1.x)
		&& (r2.y >= r1.y)
		&& (r2.w <= r1.w)
		&& (r2.h <= r1.h)
	);
}


inline
bool Shape::intersects(const SDL_Rect& r1, const SDL_Rect& r2) {
	return (SDL_HasIntersection(&r1, &r2) == SDL_TRUE);
}


inline
SDL_Rect Shape::bounds(const Circle& c) {
	return {c.x - c.r, c.y - c.r, c.r * 2, c.r * 2};
}
