#pragma once

#include "sdl_helper.h"
#include "widget.h"
#include <cstddef>
#include <memory>
#include <vector>


class BitmapButton;
class DialogData;


class Dialog : public Widget {
public:
	Dialog();
	~Dialog();
	void setData(std::shared_ptr<DialogData>);
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
	void updateContentBounds(void);

	std::vector<BitmapButton*> buttons;
	SDL_Rect contentBounds;	// this is relative to bounds
	// below bounds are relative to content bounds (inside border)
	SDL_Rect boundsTitle;
	SDL_Rect boundsTitleBar;
	SDL_Rect boundsBody;
	SDL_Rect boundsMessage;
	std::shared_ptr<DialogData> data = nullptr;
	SDL_Texture* texTitle = nullptr;
	SDL_Texture* texMessage = nullptr;
	bool isDone = false;
};


inline
bool Dialog::done() const {
	return isDone;
}
