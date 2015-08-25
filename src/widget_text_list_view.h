#pragma once

#include "color.h"
#include "text_renderer.h"
#include "widget_scroll_bar.h"
#include <cstddef>
#include <functional>
#include <string>
#include <vector>


class TextItem;


struct DrawRange {
	std::size_t lo;
	std::size_t hi;
	WidgetState state;
};


class TextListView : public Widget {
	TextListView(const TextListView&) = delete;
	void operator=(const TextListView&) = delete;
	friend class TextItem;	// only for renderText()
public:
	typedef std::function<void(TextItem*)> SelectedCallback;
	TextListView();
	~TextListView();
	void add(const std::string&);
	void setItemHeight(const int);
	int getItemHeight(void) const;
	void setSelectedCallback(SelectedCallback);
	void setScrollBarWidth(const int);
	void setBackgroundColor(const Color&);
	void setItemColors(const Color&, const Color&, const Color&);
	void setTextColor(const Color&);
	void setScrollBarStyle(const ScrollBarStyle&);
	TextRenderer* getTextRenderer(void);
	// Widget implementation
	void draw(Canvas&) override;
	void event(WidgetEvent&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	SDL_Surface* renderText(const std::string&);	// called by TextItem
	void scrollBarCallback(const int);
	void setDownItem(const std::size_t);
	int getItemWidth(void) const;
	std::size_t getItemY(const int);
	void updateDrawRange(void);
	void updateVisible(void);
	void updateScrollBarPos(void);
	void reset(void);

	TextRenderer tr;
	ScrollBar bar;
	std::vector<TextItem*> items;
	std::vector<DrawRange> drawItems;
	// called whenever change in selected item
	SelectedCallback callback;
	std::size_t visibleStart;
	std::size_t visibleEnd;
	std::size_t overItem;	// item that cursor is over
	std::size_t downItem;	// item that has been selected
	int itemHeight = 1;
	Color colBg;
	Color colItemBgOut;
	Color colItemBgOver;
	Color colItemBgDown;
	Color colText;
	bool mousePressed = false;
	bool barPressed = false;
};


inline
int TextListView::getItemHeight() const {
	return itemHeight;
}


inline
void TextListView::setSelectedCallback(SelectedCallback func) {
	callback = func;
}


inline
void TextListView::setBackgroundColor(const Color& c) {
	colBg = c;
}


inline
void TextListView::setTextColor(const Color& c) {
	colText = c;
}


inline
void TextListView::setScrollBarStyle(const ScrollBarStyle& s) {
	bar.setStyle(s);
}


inline
TextRenderer* TextListView::getTextRenderer() {
	return &tr;
}


inline
int TextListView::getItemWidth() const {
	return (bounds.w - bar.getSize().first);
}
