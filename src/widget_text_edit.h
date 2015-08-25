#pragma once

#include "color.h"
#include "sdl_helper.h"
#include "widget.h"
#include <string>


class TextRenderer;


// single-line minimal text edit
// Note: baseline is not fixed in position
class TextEdit : public Widget {
	typedef TextEdit self_type;
public:
	TextEdit();
	~TextEdit();
	void setRenderer(TextRenderer* const);
	void setOffset(const int);
	void setVertPadding(const int);
	void setStyle(const Color&, const Color&, const Color&);
	void setText(const std::string&);
	const std::string& getText(void) const;
	// Widget implementation
	void draw(Canvas&) override;
	void event(WidgetEvent&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	void updateText(void);
	void enableTextInput(void);
	void disableTextInput(void);

	std::string strText;
	SDL_Rect texBounds;
	TextRenderer* tr = nullptr;
	SDL_Texture* tex = nullptr;
	int offsetX;
	int paddingV = 3;
	Color colText;
	Color colBg;
	Color colOutline;
};


inline
const std::string& TextEdit::getText() const {
	return strText;
}
