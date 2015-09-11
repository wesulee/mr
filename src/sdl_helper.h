#pragma once

#include "sdl_header.h"
#include <string>


class Color;


namespace SDLHintValue {
	constexpr char TRUE[] = "1";
	constexpr char FALSE[] = "0";
}


class SDL {
public:
	enum class Flip {HORIZ, VERT, HORIZ_VERT};
	static bool init(void);
	static void logError(const std::string&);
	static SDL_Surface* newSurface24(const int, const int);	// 24-bit surface (no alpha)
	static SDL_Surface* newSurface32(const int, const int);	// 32-bit surface (has alpha channel)
	static SDL_Surface* newSurfaceCopyFormat(SDL_Surface*, const int, const int);
	static SDL_Texture* newTexture(SDL_Surface*);
	static SDL_Texture* toTexture(SDL_Surface*);	// frees given surface
	static bool setColorKey(SDL_Surface*, const Color&);
	static SDL_Surface* flip24(SDL_Surface*, const Flip);	// flip a 24-bit surface
	// flip contents of src rect and blit to dst (same surface, rect must be same size) (24-bit surface)
	static void copyFlip24(SDL_Surface*, const SDL_Rect&, const SDL_Rect&, const Flip);
	static void free(SDL_Surface*);
	static void free(SDL_Texture*);
	static void freeNull(SDL_Surface*);
	static void freeNull(SDL_Texture*);
	static std::string rendererFlagsToString(const Uint32);
	static Uint32 mapRGB(const SDL_PixelFormat*, const Color&);
	static Uint32 mapRGBA(const SDL_PixelFormat*, const Color&, const Uint8);
	static void clearEvents(void);
	static bool pushUserEvent(SDL_Event&);
	static SDL_Surface* copyScreen(void);
	static void renderSetViewport(SDL_Rect*);
	static void renderSetClipRect(SDL_Rect*);
	static SDL_Surface* loadBMP(const std::string&);
	static void getDim(SDL_Texture*, int&, int&);
	static void queryTexture(SDL_Texture*, Uint32*, int*, int*, int*);
	static TTF_Font* openFont(const std::string&, const int);
	static void glyphMetrics(TTF_Font*, Uint16, int*, int*, int*, int*, int*);
	static void setAlpha(SDL_Texture*, const Uint8);
	static SDL_Window* createWindow(const char*, const int, const int, const int, const int, const Uint32);
	static SDL_Renderer* createRenderer(SDL_Window*, const int, const Uint32);
	static int getNumRenderDrivers(void);
	static void getRenderDriverInfo(const int, SDL_RendererInfo*);
	static void getRendererInfo(SDL_Renderer*, SDL_RendererInfo*);
	static void setRenderDrawBlendMode(SDL_Renderer*, const SDL_BlendMode);

	static SDL_Window* window;
	static SDL_Renderer* renderer;
	static Uint32 userEventType;
	static bool targetTextureSupport;
private:
	static SDL_Surface* createSurface(int, int, int, Uint32, Uint32, Uint32, Uint32);
	static void flip24Apply(SDL_Surface*, const SDL_Rect&, SDL_Surface*, const SDL_Rect&, const Flip);
};


bool operator==(const SDL_Rect&, const SDL_Rect&);
