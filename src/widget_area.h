#pragma once

#include "widget.h"


class WidgetLayout;


// Special top-level widget in a window in which all other widgets must be contained in.
// Because WidgetArea is not supposed to be a child, the methods getPrefSize(), getMinSize(),
//   and _resize() are not allowed.
class WidgetArea : public Widget {
	WidgetArea(const WidgetArea&) = delete;
	void operator=(const WidgetArea&) = delete;
public:
	WidgetArea();
	~WidgetArea();
	void setPosition(const IntPair&);
	void setSize(const IntPair&);
	void setLayout(WidgetLayout*);
	// Widget implementation
	void draw(Canvas&) override;
	void event(WidgetEvent&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _requestResize(Widget*, const IntPair&) override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	WidgetLayout* layout = nullptr;
};
