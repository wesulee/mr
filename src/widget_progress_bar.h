#pragma once

#include "color.h"
#include "widget.h"


// horizontal
class ProgressBar : public Widget {
public:
	ProgressBar();
	~ProgressBar() = default;
	void setPrefSize(const IntPair&);
	void setBackgroundColor(const Color&);
	void setFillColor(const Color&);
	void setOutlineSize(const int);
	void setMaxValue(const int);
	void setValue(const int);
	int getValue(void) const;
	bool done(void) const;
	// Widget implementation
	void draw(Canvas&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	void resetPos(void);
	void setProgBounds(void);

	SDL_Rect progBounds;
	IntPair prefSize;
	int outlineSz = 3;
	int value = 0;
	int maxValue = 1;
	Color colBg;
	Color colFill;	// also color of outline
};


inline
int ProgressBar::getValue() const {
	return value;
}


inline
bool ProgressBar::done() const {
	return (value >= maxValue);
}
