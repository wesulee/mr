#pragma once

#include "color.h"
#include "widget.h"
#include <functional>


struct ScrollBarStyle {
	Color bg;
	Color out;
	Color over;
	Color down;
};


// Note: while content size is less than window size, set content size to window size
class AbstractScrollBar {
public:
	void setWindowSize(const int);
	void setContentSize(const int);
	void setGripOffset(int);
	int getGripOffset(void) const;
	int getMaxGripOffset(void) const;
	int getGripSize(void) const;
	int getContentOffset(void) const;
	int getMinSize(void) const;
private:
	void updateGripSize(void);

	int windowSize = 100;
	int gripSize = 100;
	int gripOffset = 0;
	int minGripSize = 10;
	int contentSize = 100;	// (minimum of windowSize)
	int contentOffset = 0;
	float ratioCG = 0;	// content offset per gripOffset
};


// Vertical scrollbar
class ScrollBar : public Widget {
public:
	typedef std::function<void(const int)> Callback;
	ScrollBar();
	ScrollBar(const ScrollBar&) = default;
	~ScrollBar() = default;
	void setStyle(const ScrollBarStyle&);
	void setContentSize(const int);
	void setCallback(Callback);
	int getContentOffset(void) const;
	// Widget implementation
	void draw(Canvas&) override;
	void event(WidgetEvent&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	void updateBarBounds(void);

	AbstractScrollBar bar;
	// called whenever contentOffset has been changed
	Callback callback;
	SDL_Rect barBounds;		// relative to bounds
	ScrollBarStyle style;
	WidgetState state = WidgetState::OUT;
	int pressOffset = 0;
	int prevContentOffset;
};


inline
int AbstractScrollBar::getGripOffset() const {
	return gripOffset;
}


inline
int AbstractScrollBar::getGripSize() const {
	return gripSize;
}


inline
int AbstractScrollBar::getContentOffset() const {
	return contentOffset;
}


inline
int AbstractScrollBar::getMaxGripOffset() const {
	return (windowSize - gripSize);
}


inline
int AbstractScrollBar::getMinSize() const {
	return minGripSize;
}


inline
void ScrollBar::setStyle(const ScrollBarStyle& s) {
	style = s;
}


inline
void ScrollBar::setCallback(Callback func) {
	callback = func;
}


inline
int ScrollBar::getContentOffset() const {
	return bar.getContentOffset();
}
