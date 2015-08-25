#include "canvas.h"
#include "constants.h"
#include "drawable.h"
#include "font.h"
#include "image.h"
#include "sprite.h"


Canvas::Canvas() {
	setColor(color, alpha);
	SDL_Rect windowRect;
	windowRect.x = 0;
	windowRect.y = 0;
	SDL_GetWindowSize(SDL::window, &windowRect.w, &windowRect.h);
	stViewports.push(windowRect);
}


void Canvas::draw(Drawable& d) {
	dst.x = d.getDrawPosX() + offsetX;
	dst.y = d.getDrawPosY() + offsetY;
	dst.w = d.getDrawWidth();
	dst.h = d.getDrawHeight();
	SDL_RenderCopy(SDL::renderer, d.getTexture(), d.getTextureBounds(), &dst);
}


void Canvas::draw(Drawable& d, const int x, const int y) {
	dst.x = x + offsetX;
	dst.y = y + offsetY;
	dst.w = d.getDrawWidth();
	dst.h = d.getDrawHeight();
	SDL_RenderCopy(SDL::renderer, d.getTexture(), d.getTextureBounds(), &dst);
}


void Canvas::draw(Image& img, const int x, const int y) {
	dst.x = x + offsetX;
	dst.y = y + offsetY;
	dst.w = img.getDrawWidth();
	dst.h = img.getDrawHeight();
	SDL_RenderCopy(SDL::renderer, img.getTexture(), nullptr, &dst);
}


void Canvas::draw(const TextImage& img, const int x, const int y) {
	dst.x = x + offsetX;
	dst.y = y + offsetY;
	dst.w = img.getWidth();
	dst.h = img.getHeight();
	SDL_RenderCopy(SDL::renderer, img.tex, NULL, &dst);
}


// draw border of size sz inside rect
void Canvas::draw(const SDL_Rect& rect, const int sz) {
	assert(sz > 0);
	SDL_Rect fillRects[4];
	// top
	fillRects[0].x = rect.x + offsetX;
	fillRects[0].y = rect.y + offsetY;
	fillRects[0].w = rect.w;
	fillRects[0].h = sz;
	// bottom
	fillRects[1].x = fillRects[0].x;
	fillRects[1].y = fillRects[0].y + rect.h - sz;
	fillRects[1].w = fillRects[0].w;
	fillRects[1].h = fillRects[0].h;
	// left
	fillRects[2].x = fillRects[0].x;
	fillRects[2].y = fillRects[0].y + sz;
	fillRects[2].w = sz;
	fillRects[2].h = rect.h - (sz * 2);
	// right
	fillRects[3].x = fillRects[0].x + rect.w - sz;
	fillRects[3].y = fillRects[2].y;
	fillRects[3].w = fillRects[2].w;
	fillRects[3].h = fillRects[2].h;
	SDL_RenderFillRects(SDL::renderer, fillRects, 4);
}


// viewport relative to current viewport
void Canvas::setRelViewport(const SDL_Rect& rect) {
	SDL_Rect nrect;
	nrect.x = stViewports.top().x + rect.x;
	nrect.y = stViewports.top().y + rect.y;
	nrect.w = rect.w;
	nrect.h = rect.h;
	stViewports.push(nrect);
	SDL::renderSetViewport(&stViewports.top());
}


SDL_Texture* Canvas::newRenderTarget() {
	assert(SDL::targetTextureSupport);
	SDL_Texture* tex = SDL_CreateTexture(
		SDL::renderer, SDL_GetWindowPixelFormat(SDL::window), SDL_TEXTUREACCESS_TARGET,
		Constants::windowWidth, Constants::windowHeight
	);
	if (tex == nullptr) {
		SDL::logError("Canvas::newRenderTarget SDL_CreateTexture");
		return tex;
	}

	if (SDL_SetRenderTarget(SDL::renderer, tex) != 0) {
		SDL::logError("Canvas::newRenderTarget SDL_SetRenderTarget");
		SDL_DestroyTexture(tex);
		return nullptr;
	}

	return tex;
}


void Canvas::clearRenderTarget() {
	if (SDL_SetRenderTarget(SDL::renderer, nullptr) != 0) {
		SDL::logError("Canvas::clearRenderTarget SDL_SetRenderTarget");
		// throw runtime_error?
	}
}
