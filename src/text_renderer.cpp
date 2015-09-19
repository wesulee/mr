#include "text_renderer.h"
#include "exception.h"
#include "font.h"
#include "game_data.h"
#include "logger.h"
#include "resource_manager.h"
#include <cassert>


TextRenderer::TextRenderer() {
	setColor(COLOR_BLACK);
}


TextRenderer::~TextRenderer() {
	freeFont();
}


void TextRenderer::setFont(const Font& f, const bool shared) {
	freeFont();
	fr = GameData::instance().resources->loadFont(f, shared);
	metrics.height = TTF_FontHeight(fr.font);
	metrics.ascent = TTF_FontAscent(fr.font);
	metrics.descent = TTF_FontDescent(fr.font);
	metrics.lineSkip = TTF_FontLineSkip(fr.font);
}


SDL_Surface* TextRenderer::render(const std::string& str) {
	return render(str.c_str());
}


SDL_Surface* TextRenderer::render(const char* cstr) {
	SDL_Surface* surf;
	switch (renderType) {
	case TextRenderType::SOLID:
		surf = TTF_RenderText_Solid(fr.font, cstr, col);
		break;
	case TextRenderType::SHADED:
		surf = TTF_RenderText_Shaded(fr.font, cstr, col, colShaded);
		break;
	case TextRenderType::BLENDED:
		surf = TTF_RenderText_Blended(fr.font, cstr, col);
		break;
	default:
		surf = nullptr;
		assert(false);
		break;
	}
	if (surf == nullptr)
		Logger::instance().exit(SDLError{"unable to render text", SDLFunc::TTF_RenderText_});
	return surf;
}


SDL_Surface* TextRenderer::renderWrap(const std::string& str, const int width) {
	assert(width > 0);
	SDL_Surface* surf = TTF_RenderText_Blended_Wrapped(fr.font, str.c_str(), col, static_cast<Uint32>(width));
	if (surf == nullptr)
		Logger::instance().exit(SDLError{"unable to render text", SDLFunc::TTF_RenderText_});
	return surf;
}


void TextRenderer::size(const std::string& text, int& width, int& height) {
	if (TTF_SizeText(fr.font, text.c_str(), &width, &height) != 0)
		Logger::instance().exit(SDLError{"unable to get text size", SDLFunc::TTF_SizeText});
}


void TextRenderer::freeFont() {
	if (fr.font != nullptr) {
		GameData::instance().resources->unloadFont(fr);
		fr.font = nullptr;
	}
}
