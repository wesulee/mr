#include "sdl_helper.h"
#include "color.h"
#include "constants.h"
#include "exception.h"
#include "logger.h"
#include <cassert>
#include <limits>


// default values
SDL_Window* SDL::window = nullptr;
SDL_Renderer* SDL::renderer = nullptr;
Uint32 SDL::userEventType = std::numeric_limits<Uint32>::max();
bool SDL::targetTextureSupport = false;


namespace SDLHelper {
	constexpr char rendererFlag_SOFTWARE[] = "SOFTWARE";
	constexpr char rendererFlag_ACCELERATED[] = "ACCELERATED";
	constexpr char rendererFlag_PRESENTVSYNC[] = "PRESENTVSYNC";
	constexpr char rendererFlag_TARGETTEXTURE[] = "TARGETTEXTURE";

	inline
	void appendRendererFlagString(std::string& str, const char* flagName) {
		if (!str.empty())
			str += ' ';
		str += flagName;
	}
}


bool SDL::init() {
	if (SDL_Init(SDL_INIT_TIMER | SDL_INIT_VIDEO) != 0) {
		logError("SDL_Init");
		return false;
	}
	SDL_StopTextInput();
	if (TTF_Init() != 0) {
		logError("TTF_Init");
		return false;
	}
	userEventType = SDL_RegisterEvents(1);
	if (userEventType == std::numeric_limits<Uint32>::max()) {
		logError("SDL_RegisterEvents");
		return false;
	}
	return true;
}


void SDL::logError(const std::string& msg) {
	std::string str{msg};
	str += " error: ";
	str += SDL_GetError();
	Logger::instance().log(str);
}


SDL_Surface* SDL::newSurface24(const int w, const int h) {
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xFF000000;
	gmask = 0x00FF0000;
	bmask = 0x0000FF00;
#else
	rmask = 0x000000FF;
	gmask = 0x0000FF00;
	bmask = 0x00FF0000;
#endif
	amask = 0x00000000;
	return createSurface(w, h, 24, rmask, gmask, bmask, amask);
}


SDL_Surface* SDL::newSurface32(const int w, const int h) {
	Uint32 rmask, gmask, bmask, amask;
#if SDL_BYTEORDER == SDL_BIG_ENDIAN
	rmask = 0xFF000000;
	gmask = 0x00FF0000;
	bmask = 0x0000FF00;
	amask = 0x000000FF;
#else
	rmask = 0x000000FF;
	gmask = 0x0000FF00;
	bmask = 0x00FF0000;
	amask = 0xFF000000;
#endif
	return createSurface(w, h, 32, rmask, gmask, bmask, amask);
}


SDL_Surface* SDL::newSurfaceCopyFormat(SDL_Surface* surf, const int w, const int h) {
	return SDL_CreateRGBSurface(
		0, w, h, surf->format->BitsPerPixel,
		surf->format->Rmask,
		surf->format->Gmask,
		surf->format->Bmask,
		surf->format->Amask
	);
}


SDL_Texture* SDL::newTexture(SDL_Surface* surf) {
	return SDL_CreateTextureFromSurface(renderer, surf);
}


SDL_Texture* SDL::toTexture(SDL_Surface* surf) {
	assert(surf != nullptr);
	SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
	free(surf);
	if (tex == nullptr)
		logAndExit(SDLError{"unable to convert surface to texture", SDLFunc::SDL_CreateTextureFromSurface});
	return tex;
}


bool SDL::setColorKey(SDL_Surface* surf, const Color& c) {
	return (SDL_SetColorKey(surf, SDL_TRUE, SDL::mapRGB(surf->format, c)) == 0);
}


/*
SDL_Surface* SDL::flip24(SDL_Surface* surf, const Flip flip) {
	SDL_Surface* fsurf = newSurfaceCopyFormat(surf, surf->w, surf->h);
	if (fsurf == nullptr) {
		logError("SDL::flip24 newSurfaceCopyFormat");
		return nullptr;
	}
	if (SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
	if (SDL_MUSTLOCK(fsurf)) SDL_LockSurface(fsurf);
	flip24Apply(surf, {0, 0, surf->w, surf->h}, fsurf, {0, 0, surf->w, surf->h}, flip);
	if (SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);
	if (SDL_MUSTLOCK(fsurf)) SDL_UnlockSurface(fsurf);
	return fsurf;
}


void SDL::copyFlip24(SDL_Surface* surf, const SDL_Rect& src, const SDL_Rect& dst, const Flip flip) {
	if (SDL_MUSTLOCK(surf)) SDL_LockSurface(surf);
	flip24Apply(surf, src, surf, dst, flip);
	if (SDL_MUSTLOCK(surf)) SDL_UnlockSurface(surf);
}
*/


void SDL::free(SDL_Surface* surf) {
	SDL_FreeSurface(surf);
}


void SDL::free(SDL_Texture* tex) {
	SDL_DestroyTexture(tex);
}


void SDL::freeNull(SDL_Surface* surf) {
	if (surf != nullptr)
		SDL_FreeSurface(surf);
}


void SDL::freeNull(SDL_Texture* tex) {
	if (tex != nullptr)
		SDL_DestroyTexture(tex);
}


std::string SDL::rendererFlagsToString(const Uint32 flags) {
	using namespace SDLHelper;
	std::string str;
	if (flags & SDL_RENDERER_SOFTWARE)
		appendRendererFlagString(str, rendererFlag_SOFTWARE);
	if (flags & SDL_RENDERER_ACCELERATED)
		appendRendererFlagString(str, rendererFlag_ACCELERATED);
	if (flags & SDL_RENDERER_PRESENTVSYNC)
		appendRendererFlagString(str, rendererFlag_PRESENTVSYNC);
	if (flags & SDL_RENDERER_TARGETTEXTURE)
		appendRendererFlagString(str, rendererFlag_TARGETTEXTURE);
	return str;
}


Uint32 SDL::mapRGB(const SDL_PixelFormat* format, const Color& c) {
	return SDL_MapRGB(format, c.R, c.G, c.B);
}


Uint32 SDL::mapRGBA(const SDL_PixelFormat* format, const Color& c, const Uint8 a) {
	return SDL_MapRGBA(format, c.R, c.G, c.B, a);
}


void SDL::clearEvents() {
	SDL_Event e;
	while (SDL_PollEvent(&e))
		;
}


bool SDL::pushUserEvent(SDL_Event& e) {
	e.type = userEventType;
	return (SDL_PushEvent(&e) >= 0);
}


// copy contents of window to a surface
SDL_Surface* SDL::copyScreen() {
	SDL_Surface* surf = newSurface32(Constants::windowWidth, Constants::windowHeight);
	if (SDL_RenderReadPixels(renderer, nullptr, surf->format->format, surf->pixels, surf->pitch) != 0)
		logAndExit(SDLError{"SDL::copyScreen", SDLFunc::SDL_RenderReadPixels});
	return surf;
}


void SDL::renderSetViewport(SDL_Rect* rect) {
	if (SDL_RenderSetViewport(renderer, rect) != 0)
		logAndExit(SDLError{"unable to set viewport", SDLFunc::SDL_RenderSetViewport});
}


void SDL::renderSetClipRect(SDL_Rect* rect) {
	if (SDL_RenderSetClipRect(renderer, rect) != 0)
		logAndExit(SDLError{"unable to set clip rect", SDLFunc::SDL_RenderSetClipRect});
}


SDL_Surface* SDL::loadBMP(const std::string& path) {
	SDL_Surface* surf = SDL_LoadBMP(path.c_str());
	if (surf == nullptr)
		logAndExit(SDLError{"unable to load image", SDLFunc::SDL_LoadBMP});
	return surf;
}


void SDL::getDim(SDL_Texture* tex, int& w, int& h) {
	queryTexture(tex, nullptr, nullptr, &w, &h);
}


void SDL::queryTexture(SDL_Texture* tex, Uint32* format, int* access, int* w, int* h) {
	if (SDL_QueryTexture(tex, format, access, w, h) != 0)
		logAndExit(SDLError{"query texture failure", SDLFunc::SDL_QueryTexture});
}


void SDL::glyphMetrics(TTF_Font* f, Uint16 c, int* minX, int* maxX, int* minY, int* maxY, int* advance) {
	if (TTF_GlyphMetrics(f, c, minX, maxX, minY, maxY, advance) != 0)
		logAndExit(SDLError{"glyph metrics failure", SDLFunc::TTF_GlyphMetrics});
}


void SDL::setAlpha(SDL_Texture* tex, const Uint8 alpha) {
	if (SDL_SetTextureAlphaMod(tex, alpha) != 0)
		logAndExit(SDLError{"texture alpha mod failure", SDLFunc::SDL_SetTextureAlphaMod});
}


SDL_Window* SDL::createWindow(const char* title, const int x, const int y, const int w, const int h, const Uint32 flags) {
	SDL_Window* window = SDL_CreateWindow(title, x, y, w, h, flags);
	if (window == nullptr)
		logAndExit(SDLError{"create window failure", SDLFunc::SDL_CreateWindow});
	return window;
}


SDL_Renderer* SDL::createRenderer(SDL_Window* w, const int i, const Uint32 flags) {
	SDL_Renderer* renderer = SDL_CreateRenderer(w, i, flags);
	if (renderer == nullptr)
		logAndExit(SDLError{"create renderer failure", SDLFunc::SDL_CreateRenderer});
	return renderer;
}


int SDL::getNumRenderDrivers() {
	const int n = SDL_GetNumRenderDrivers();
	if (n < 1)
		logAndExit(SDLError{"unable to get number of render drivers", SDLFunc::SDL_GetNumRenderDrivers});
	return n;
}


void SDL::getRenderDriverInfo(const int i, SDL_RendererInfo* info) {
	if (SDL_GetRenderDriverInfo(i, info) != 0)
		logAndExit(SDLError{"unable to get render driver info", SDLFunc::SDL_GetRenderDriverInfo});
}


void SDL::getRendererInfo(SDL_Renderer* renderer, SDL_RendererInfo* info) {
	if (SDL_GetRendererInfo(renderer, info) != 0)
		logAndExit(SDLError{"unable to get renderer info", SDLFunc::SDL_GetRendererInfo});
}


void SDL::setRenderDrawBlendMode(SDL_Renderer* renderer, const SDL_BlendMode mode) {
	if (SDL_SetRenderDrawBlendMode(renderer, mode) != 0)
		logAndExit(SDLError{"unable to set blend mode", SDLFunc::SDL_SetRenderDrawBlendMode});
}


SDL_Surface* SDL::createSurface(int w, int h, int depth, Uint32 r, Uint32 g, Uint32 b, Uint32 a) {
	SDL_Surface* surf = SDL_CreateRGBSurface(0, w, h, depth, r, g, b, a);
	if (surf == nullptr)
		logAndExit(SDLError{"unable to create new surface", SDLFunc::SDL_CreateRGBSurface});
	return surf;
}


/*
// flip pixels of s1 contained in src and copy onto s2 contained in dst
// see http://sdl.beuc.net/sdl.wiki/Pixel_Access for details
void SDL::flip24Apply(SDL_Surface* s1, const SDL_Rect& src, SDL_Surface* s2, const SDL_Rect& dst, const Flip flip) {
	assert(s1->format->BytesPerPixel == 3);
	assert(s2->format->BytesPerPixel == 3);
	assert(src.w == dst.w && src.h == dst.h);
	Uint8* psrc;
	Uint8* pdst;
	Uint32 pixel;
	// refer to positions by offsets from origin (top-left of rect)
	int dxo = 0, dyo = 0;		// dst x, y offset
	for (int xo = 0; xo < src.w; ++xo) {		// src offset
		for (int yo = 0; yo < src.h; ++yo) {
			psrc = (Uint8*)s1->pixels + (src.y + yo) * s1->pitch + (src.x + xo) * 3;
			#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				pixel = psrc[0] << 16 | psrc[1] << 8 | psrc[2];
			#else
				pixel = psrc[0] | psrc[1] << 8 | psrc[2] << 16;
			#endif
			switch (flip) {
			case Flip::HORIZ:
				dxo = dst.w - 1 - xo;
				dyo = yo;
				break;
			case Flip::VERT:
				dxo = xo;
				dyo = dst.h - 1 - yo;
				break;
			case Flip::HORIZ_VERT:
				dxo = dst.w - 1 - xo;
				dyo = dst.h - 1 - yo;
				break;
			}
			pdst = (Uint8*)s2->pixels + (dst.y + dyo) * s2->pitch + (dst.x + dxo) * 3;
			#if SDL_BYTEORDER == SDL_BIG_ENDIAN
				pdst[0] = (pixel >> 16) & 0xFF;
				pdst[1] = (pixel >> 8) & 0xFF;
				pdst[2] = pixel & 0xFF;
			#else
				pdst[0] = pixel & 0xFF;
				pdst[1] = (pixel >> 8) & 0xFF;
				pdst[2] = (pixel >> 16) & 0xFF;
			#endif
		}
	}
}
*/


bool operator==(const SDL_Rect& r1, const SDL_Rect& r2) {
	return (
		(r1.x == r2.x)
		&& (r1.y == r2.y)
		&& (r1.w == r2.w)
		&& (r1.h == r2.h)
	);
}
