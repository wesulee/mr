#pragma once

#include "sdl_helper.h"


class Color;


// Draw various shapes
// Note: since I couldn't get SDL2_gfx to work, I'll implement what I need.
class ShapeRenderer {
public:
	static SDL_Surface* circle(const Color&, const Color&, const int);
private:
	static void circBoundLine(const float, const float, int*, int*);
	static bool circDrawLine(SDL_Surface*, const int, const int, const int);

	static SDL_Rect dst;
	static Uint32 color;
};
