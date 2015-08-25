#pragma once

#include "widget.h"
#include <string>


class TextListView;
class TextRenderer;


// This is a specialized widget only to be constructed by TextListView.
class TextItem : public WidgetWithVisibility {
	TextItem(const TextItem&) = delete;
	void operator=(const TextItem&) = delete;
public:
	TextItem();
	~TextItem();
	void setText(const std::string&);
	const std::string& getText(void) const;
	// Widget implementation
	void draw(Canvas&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void setVisible(const bool) override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	void setText(void);
	TextListView* getParent2(void);

	std::string text;
	SDL_Rect textBounds;
	SDL_Texture* tex = nullptr;
};


inline
const std::string& TextItem::getText() const {
	return text;
}
