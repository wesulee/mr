#pragma once

#include "color.h"
#include "sdl_helper.h"
#include "widget.h"
#include <string>


class TextRenderer;


class WidgetText : public Widget {
public:
	WidgetText();
	~WidgetText();
	void setRenderer(TextRenderer*);
	void setText(const std::string&);
	const std::string& getText(void) const;
	void setTextColor(const Color&);
	void enableBackground(void);
	void enableBackground(const Color&);
	void disableBackground(void);
	IntPair getTextSize(void) const;
	// Widget implementation
	void draw(Canvas&) override;
	void event(WidgetEvent&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	void render(void);

	std::string text;
	SDL_Texture* tex;
	TextRenderer* tr;
	IntPair texSz;		// generated texture size
	Color colText;
	Color colBg;
	bool drawBg = false;
};


inline
const std::string& WidgetText::getText() const {
	return text;
}


inline
IntPair WidgetText::getTextSize() const {
	return texSz;
}
