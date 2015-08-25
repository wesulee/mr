#include "shapes.h"
#include "utility.h"


bool Shape::intersects(const SDL_Rect& r, const Circle& c) {
	SDL_Rect rc = Shape::bounds(c);
	if (intersects(r, rc))
		return true;
	const int r2 = square(c.r);
	// check top left, bottom right, top right, bottom left
	return (
		(square(c.x - r.x) + square(c.y - r.y) < r2)
		|| (square(c.x - (r.x + r.w)) + square(c.y - (r.y + r.h) < r2))
		|| (square(c.x - (r.x + r.w)) + square(c.y - r.y) < r2)
		|| (square(c.x - r.x) + square(c.y - (r.y + r.h)) < r2)
	);
}


bool Shape::intersects(const Circle& c1, const Circle& c2) {
	return (square(c1.r - c2.r) >= (square(c1.x - c2.x) + square(c1.y - c2.y)));
}
