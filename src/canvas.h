#pragma once

#include "color.h"
#include "exception.h"
#include "sdl_helper.h"
#include <cassert>
#include <stack>
#include <utility>


class Drawable;
class Image;
class TextImage;


// utility to get alpha at compile time
// Ex: getAlpha<50>() returns value for 50% alpha
template<unsigned int A>	// percentage alpha
constexpr Uint8 getAlpha() {
	static_assert(A <= 100, "percentage required [0, 100]");
	return static_cast<Uint8>(static_cast<float>(A) / 100.0f * SDL_ALPHA_OPAQUE);
}


class Canvas {
public:
	typedef std::pair<Color, Uint8> ColorState;
	Canvas();
	~Canvas() = default;
	Color getColor(void) const;
	void setColor(const Color&);
	void setColor(const Color&, const Uint8);
	Uint8 getAlpha(void) const;
	void setAlpha(const Uint8);
	void clearScreen(void);
	void present(void);
	void draw(Drawable&);
	void draw(Drawable&, const int, const int);
	void draw(Image&, const int, const int);
	void draw(const TextImage&, const int, const int);
	void draw(SDL_Texture*, SDL_Rect*);
	void draw(const SDL_Rect&);
	void draw(const SDL_Rect&, const int);
	void fillRect(const SDL_Rect&);
	void fillRect(const int, const int, const int, const int);
	void setViewport(const SDL_Rect&);
	void setRelViewport(const SDL_Rect&);
	void clearViewport(void);
	void setClip(SDL_Rect*);
	SDL_Texture* newRenderTarget(void);
	void clearRenderTarget(void);
	// Offsets applied to all drawing
	// Used because negative viewport origin does not work
	void setOffset(const int, const int);
	void clearOffset(void);
	void setColorState(const ColorState&);
	ColorState getColorState(void) const;
private:
	std::stack<std::pair<int, int>> stOffsets;
	std::stack<SDL_Rect> stViewports;
	SDL_Rect dst;	// used by various functions
	int offsetX = 0;
	int offsetY = 0;
	Color color;
	Uint8 alpha = SDL_ALPHA_OPAQUE;
};


inline
Color Canvas::getColor() const {
	return color;
}


inline
void Canvas::setColor(const Color& c) {
	color = c;
	SDL_SetRenderDrawColor(SDL::renderer, color.R, color.G, color.B, alpha);
}


inline
void Canvas::setColor(const Color& c, const Uint8 a) {
	color = c;
	alpha = a;
	SDL_SetRenderDrawColor(SDL::renderer, color.R, color.G, color.B, alpha);
}


inline
Uint8 Canvas::getAlpha() const {
	return alpha;
}


inline
void Canvas::setAlpha(const Uint8 a) {
	alpha = a;
	SDL_SetRenderDrawColor(SDL::renderer, color.R, color.G, color.B, alpha);
}


inline
void Canvas::clearScreen() {
	// Note: this ignores viewport
	SDL_RenderClear(SDL::renderer);
}


inline
void Canvas::present() {
	SDL_RenderPresent(SDL::renderer);
}



inline
void Canvas::fillRect(const SDL_Rect& r) {
	dst.x = r.x + offsetX;
	dst.y = r.y + offsetY;
	dst.w = r.w;
	dst.h = r.h;
	SDL_RenderFillRect(SDL::renderer, &dst);
}


inline
void Canvas::fillRect(const int x, const int y, const int w, const int h) {
	dst.x = x + offsetX;
	dst.y = y + offsetY;
	dst.w = w;
	dst.h = h;
	SDL_RenderFillRect(SDL::renderer, &dst);
}


inline
void Canvas::draw(SDL_Texture* tex, SDL_Rect* dest) {
	dst.x = dest->x + offsetX;
	dst.y = dest->y + offsetY;
	dst.w = dest->w;
	dst.h = dest->h;
	SDL_RenderCopy(SDL::renderer, tex, nullptr, &dst);
}


inline
void Canvas::draw(const SDL_Rect& rect) {
	dst.x = rect.x + offsetX;
	dst.y = rect.y + offsetY;
	dst.w = rect.w;
	dst.h = rect.h;
	SDL_RenderDrawRect(SDL::renderer, &dst);
}


// reassigns screen to rect
inline
void Canvas::setViewport(const SDL_Rect& rect) {
	stViewports.push(rect);
	SDL::renderSetViewport(&stViewports.top());
}


inline
void Canvas::clearViewport() {
	assert(!stViewports.empty());
	stViewports.pop();
	SDL::renderSetViewport(&stViewports.top());
}


// draw only inside clip
inline
void Canvas::setClip(SDL_Rect* rect) {
	SDL::renderSetClipRect(rect);
}


inline
void Canvas::setOffset(const int x, const int y) {
	stOffsets.push(std::make_pair(offsetX, offsetY));
	offsetX = x;
	offsetY = y;
}


inline
void Canvas::clearOffset() {
	assert(!stOffsets.empty());
	offsetX = stOffsets.top().first;
	offsetY = stOffsets.top().second;
	stOffsets.pop();
}


inline
void Canvas::setColorState(const ColorState& cs) {
	setColor(cs.first, cs.second);
}


inline
Canvas::ColorState Canvas::getColorState() const {
	return std::make_pair(color, alpha);
}
