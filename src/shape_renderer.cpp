#include "shape_renderer.h"
#include "color.h"
#include <cassert>
#include <cmath>


SDL_Rect ShapeRenderer::dst = SDL_Rect{0, 0, 0, 0};
Uint32 ShapeRenderer::color = 0;


SDL_Surface* ShapeRenderer::circle(const Color& fg, const Color& bg, const int radius) {
	assert(radius > 0);
	//! TODO: figure out why colors look off when calling newSurface24()
	SDL_Surface* surf = SDL::newSurface32(radius * 2 + 1, radius * 2 + 1);
	// fill with background color
	color = SDL::mapRGBA(surf->format, bg, SDL_ALPHA_OPAQUE);
	SDL_FillRect(surf, nullptr, color);
	color = SDL::mapRGBA(surf->format, fg, SDL_ALPHA_OPAQUE);

	// see
	// http://content.gpwiki.org/index.php/SDL:Tutorials:Drawing_and_Filling_Circles
	// for basic idea on algorithm
	const float cenX = static_cast<float>(surf->w) / 2;
	int drawLeft, drawRight;
	float width;
	for (int h = 1; h <= radius; ++h) {
		width = std::sqrt(8 * radius * h - 4 * h * h);
		circBoundLine(cenX, width, &drawLeft, &drawRight);
		circDrawLine(surf, drawLeft, drawRight, h);
		circDrawLine(surf, drawLeft, drawRight, surf->h - h);
	}

	return surf;
}


void ShapeRenderer::circBoundLine(const float cenX, const float width, int* left, int* right) {
	assert((left != nullptr) && (right != nullptr));
	// set left
	int partInt = static_cast<int>(cenX - width / 2);
	float partDec = cenX - width / 2 - partInt;
	if (partDec < 0.5f)
		*left = partInt;
	else
		*left = partInt + 1;
	// set right
	partInt = static_cast<int>(cenX + width / 2);
	partDec = cenX + width / 2 - partInt;
	if (partDec >= 0.5f)
		*right = partInt + 1;
	else
		*right = partInt;
	assert(*left <= *right);
}


bool ShapeRenderer::circDrawLine(SDL_Surface* s, const int x0, const int x1, const int y) {
	dst.x = x0;
	dst.w = x1 - x0 + 1;
	dst.y = y;
	dst.h = 1;
	return (SDL_FillRect(s, &dst, color) == 0);
}
