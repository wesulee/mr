#include "ui.h"
#include "canvas.h"
#include "font_resource.h"
#include "text_renderer.h"
#include <algorithm>
#include <cassert>
#include <cmath>


Color Gradient::getFront() const {
	return front;
}


Color Gradient::getBack() const {
	return back;
}


void Gradient::setFront(const Color& c) {
	front = c;
}


void Gradient::setBack(const Color& c) {
	back = c;
}


void Gradient::add(const float pos, const Color& c) {
	assert((pos > 0) && (pos < 1));
	sorted = false;
	pairs.push_back(std::make_pair(pos, c));
}


std::vector<Color> Gradient::generate(const int length) {
	assert(length >= 2);
	if (!sorted) {
		std::sort(pairs.begin(), pairs.end(), GradCmp());
		sorted = true;
	}

	std::vector<Color> colors;
	colors.reserve(static_cast<std::size_t>(length));

	if (pairs.empty()) {
		grad(colors, getFront(), getBack(), length);
		colors.push_back(getBack());
		return colors;
	}

	auto it = pairs.cbegin();
	Color start = getFront();
	Color end;
	int cDist = 0;		// current distance gone
	int dist;			// passed to grad()
	float prevDist = 0;
	for (auto itEnd = pairs.cend(); it != itEnd; ++it) {
		end = (*it).second;
		dist = static_cast<int>(((*it).first - prevDist) * length);
		grad(colors, start, end, dist);
		cDist += dist;
		start = end;
		prevDist = (*it).first;
	}

	end = getBack();
	dist = length - cDist - 1;
	grad(colors, start, end, dist);
	colors.push_back(end);
	assert(static_cast<int>(colors.size()) == length);

	return colors;
}


void Gradient::grad(std::vector<Color>& v, const Color& start, const Color& end, const int ticks) {
	// appends 'ticks' colors to v, [start, end)
	assert(ticks >= 1);
	v.push_back(start);
	int ticksLeft = ticks-1;
	Color tmpColor;
	const float dR = (static_cast<float>(end.R) - start.R) / ticks;
	const float dG = (static_cast<float>(end.G) - start.G) / ticks;
	const float dB = (static_cast<float>(end.B) - start.B) / ticks;
	float cR = static_cast<float>(start.R);
	float cG = static_cast<float>(start.G);
	float cB = static_cast<float>(start.B);
	while (ticksLeft > 0) {
		cR += dR;
		cG += dG;
		cB += dB;
		tmpColor.R = static_cast<uint8_t>(std::round(cR));
		tmpColor.G = static_cast<uint8_t>(std::round(cG));
		tmpColor.B = static_cast<uint8_t>(std::round(cB));
		v.push_back(tmpColor);
		--ticksLeft;
	}
}


// render a font with a vertical gradient (top-down)
SDL_Surface* renderSolidTextGradient(FontResource& fr, const std::string& str, Gradient& g) {
	assert(fr.font != nullptr);
	std::string exceptionMessage{"Error rendering text gradient."};
	Color tmpColor;

	SDL_Surface* textSurf = TTF_RenderText_Blended(fr.font, str.c_str(), {255, 255, 255, SDL_ALPHA_OPAQUE});
	if (textSurf == nullptr) {
		SDL::logError("renderSolidTextGradient TTF_RenderText_Solid");
		throw std::runtime_error(exceptionMessage);
	}
	assert(textSurf->w > 0);
	assert(textSurf->h > 0);

	auto colors = g.generate(textSurf->h);

	if (SDL_MUSTLOCK(textSurf))
		SDL_LockSurface(textSurf);
	Uint32* pixels = static_cast<Uint32*>(textSurf->pixels);
	Uint32* pixel = pixels;
	Uint8 R, G, B, A;
	for (int y = 0; y < textSurf->h; ++y) {
		for (int x = 0; x < textSurf->w; ++x, ++pixel) {
			SDL_GetRGBA(*pixel, textSurf->format, &R, &G, &B, &A);
			*pixel = SDL::mapRGBA(textSurf->format, colors[static_cast<std::size_t>(y)], A);
		}
	}
	if (SDL_MUSTLOCK(textSurf))
		SDL_UnlockSurface(textSurf);

	return textSurf;
}


int StyledRectangle::getWidth() const {
	return rectWidth;
}


int StyledRectangle::getHeight() const {
	return rectHeight;
}


void StyledRectangle::setBackgroundColor(const Color& c) {
	backgroundColor = c;
}


void StyledRectangle::setBackgroundAlpha(const Uint8 a) {
	backgroundAlpha = a;
}


void StyledRectangle::setOutline(const Color& c, const Uint8 a, const int s) {
	assert(s >= 0);
	outlineColor = c;
	outlineAlpha = a;
	outlineSize = s;
}


void StyledRectangle::setTextRenderer(TextRenderer* p) {
	tr = p;
}


void StyledRectangle::setTextColor(const Color& c) {
	textColor = c;
}


void StyledRectangle::setText(const std::string& str) {
	assert(tr != nullptr);
	text = str;
	tr->size(text, textWidth, textHeight);
	rectWidth = textWidth;
	rectHeight = textHeight;
	textDst.x = 0;
	textDst.y = 0;
	textDst.w = textWidth;
	textDst.h = textHeight;
}


void StyledRectangle::setSize(const int w, const int h) {
	assert((w > 0) && (h > 0));
	rectWidth = w;
	rectHeight = h;
}


void StyledRectangle::setTextResize(bool b) {
	textResize = b;
}


SDL_Surface* StyledRectangle::generate() {
	setTextBounds();
	SDL_Surface* mainSurf = nullptr;
	SDL_Rect tmpRect;
	Uint32 tmpColor;
	if ((rectWidth == 0) || (rectHeight == 0))
		return nullptr;
	mainSurf = SDL::newSurface32(rectWidth, rectHeight);
	// draw background/outline
	if (outlineSize != 0) {
		// draw background
		tmpColor = SDL::mapRGBA(mainSurf->format, backgroundColor, backgroundAlpha);
		tmpRect.x = outlineSize;
		tmpRect.y = outlineSize;
		tmpRect.w = rectWidth - (outlineSize * 2);
		tmpRect.h = rectHeight - (outlineSize * 2);
		SDL_FillRect(mainSurf, &tmpRect, tmpColor);
		// outline top
		tmpColor = SDL::mapRGBA(mainSurf->format, outlineColor, outlineAlpha);
		tmpRect.x = 0;
		tmpRect.y = 0;
		tmpRect.w = rectWidth;
		tmpRect.h = outlineSize;
		SDL_FillRect(mainSurf, &tmpRect, tmpColor);
		// outline bottom
		tmpRect.y = rectHeight - outlineSize;
		SDL_FillRect(mainSurf, &tmpRect, tmpColor);
		// outline left
		tmpRect.y = outlineSize;
		tmpRect.w = outlineSize;
		tmpRect.h = rectHeight - (outlineSize * 2);
		SDL_FillRect(mainSurf, &tmpRect, tmpColor);
		// outline right
		tmpRect.x = rectWidth - outlineSize;
		SDL_FillRect(mainSurf, &tmpRect, tmpColor);
	}
	else {
		tmpColor = SDL::mapRGBA(mainSurf->format, backgroundColor, backgroundAlpha);
		tmpRect.x = 0;
		tmpRect.y = 0;
		tmpRect.w = rectWidth;
		tmpRect.h = rectHeight;
		SDL_FillRect(mainSurf, &tmpRect, tmpColor);
	}

	tr->setColor(textColor);
	SDL_Surface* textSurf = tr->render(text);
	SDL_BlitSurface(textSurf, nullptr, mainSurf, &textDst);
	SDL_FreeSurface(textSurf);
	return mainSurf;
}


void StyledRectangle::setTextBounds() {
	if (textResize) {
		const float rectRatio = static_cast<float>(rectWidth) / rectHeight;
		const float textRatio = static_cast<float>(textWidth) / textHeight;
		if (textRatio > rectRatio) {
			textDst.w = rectWidth;
			textDst.h = static_cast<int>(std::round(rectWidth / rectRatio));
		}
		else {
			textDst.w = static_cast<int>(std::round(rectHeight * rectRatio));
			textDst.h = rectHeight;
		}
	}
	else {
		textDst.w = textWidth;
		textDst.h = textHeight;
	}
	textDst.x = (rectWidth - textDst.w) / 2;
	textDst.y = (rectHeight - textDst.h) / 2;
}
