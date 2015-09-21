#pragma once

#include "sdl_helper.h"
#include "widget.h"
#include "widget_layout.h"
#include <cstddef>
#include <memory>


class DialogData;


class Dialog : public Widget {
	typedef Dialog self_type;
public:
	Dialog(std::shared_ptr<DialogData>);
	~Dialog();
	bool done(void) const;
	// Widget implementation
	void draw(Canvas&) override;
	void event(WidgetEvent&) override;
	IntPair getPrefSize(void) const override;
	IntPair getMinSize(void) const override;
	void _requestResize(Widget*, const IntPair&) override;
	void _resize(const IntPair&, const WidgetResizeFlag) override;
private:
	void buttonCallback(const std::size_t);

	VerticalLayout layout;
	std::shared_ptr<DialogData> data = nullptr;
	bool isDone = false;
};


inline
bool Dialog::done() const {
	return isDone;
}
