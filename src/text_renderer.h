#pragma once

#include "color.h"
#include "font_resource.h"
#include "sdl_helper.h"


class Font;


enum class TextRenderType {SOLID, SHADED, BLENDED};


struct FontMetrics {
	int height;		// max height
	int ascent;
	int descent;	// this is negative
	int lineSkip;	// recommended height of a line
};


class TextRenderer {
	TextRenderer(const TextRenderer&) = delete;
	void operator=(const TextRenderer&) = delete;
public:
	TextRenderer();
	~TextRenderer();
	void setFont(const Font&, const bool = true);
	void setRenderType(const TextRenderType);
	void setColor(const Color&);
	void setShadedBgColor(const Color&);
	SDL_Surface* render(const std::string&);
	SDL_Surface* render(const char*);
	void size(const std::string&, int&, int&);
	const FontMetrics& getMetrics(void) const;
	void freeFont(void);
private:
	FontResource fr;
	FontMetrics metrics;
	TextRenderType renderType = TextRenderType::BLENDED;
	SDL_Color col;
	SDL_Color colShaded;
};


inline
void TextRenderer::setRenderType(const TextRenderType t) {
	renderType = t;
}


inline
void TextRenderer::setColor(const Color& c) {
	col = {c.R, c.G, c.B, SDL_ALPHA_OPAQUE};
}


inline
void TextRenderer::setShadedBgColor(const Color& c) {
	colShaded = {c.R, c.G, c.B, SDL_ALPHA_OPAQUE};
}


inline
const FontMetrics& TextRenderer::getMetrics() const {
	return metrics;
}
