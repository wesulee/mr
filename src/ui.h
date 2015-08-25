#pragma once

#include "color.h"
#include "drawable.h"
#include "sdl_helper.h"
#include "shapes.h"
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>


class FontResource;
class Gradient;
class TextRenderer;


SDL_Surface* renderSolidTextGradient(FontResource&, const std::string&, Gradient&);


class Gradient {
	typedef std::pair<float, Color> GradPair;
	struct GradCmp {
		bool operator()(const GradPair& a, const GradPair& b) {
			return (a.first < b.first);
		}
	};
public:
	Gradient() = default;
	~Gradient() = default;
	Color getFront(void) const;
	Color getBack(void) const;
	void setFront(const Color&);
	void setBack(const Color&);
	void add(const float, const Color&);
	std::vector<Color> generate(const int);
private:
	static void grad(std::vector<Color>&, const Color&, const Color&, const int);

	std::vector<GradPair> pairs;
	Color front;
	Color back;
	bool sorted = false;
};


class StyledRectangle {
public:
	StyledRectangle() = default;
	~StyledRectangle() {}
	int getWidth(void) const;
	int getHeight(void) const;
	void setBackgroundColor(const Color&);
	void setBackgroundAlpha(const Uint8);
	void setOutline(const Color&, const Uint8, const int);
	void setTextRenderer(TextRenderer*);
	void setTextColor(const Color&);
	// setting text will always resize rect to size of text
	void setText(const std::string&);
	void setSize(const int, const int);
	// resize the text to fit inside rectangle?
	void setTextResize(bool);
	SDL_Surface* generate(void);
private:
	void setTextBounds(void);

	std::string text;
	SDL_Rect textDst = {0, 0, 0, 0};
	TextRenderer* tr = nullptr;
	int outlineSize = 0;
	int textWidth = 0;
	int textHeight = 0;
	int rectWidth = 0;
	int rectHeight = 0;
	Color backgroundColor;
	Color outlineColor;
	Color textColor;
	Uint8 backgroundAlpha = SDL_ALPHA_OPAQUE;
	Uint8 outlineAlpha = SDL_ALPHA_OPAQUE;
	bool textResize = false;
};
