#include "widget_dialog.h"
#include "canvas.h"
#include "dialog_data.h"
#include "game_data.h"
#include "resource_manager.h"
#include "text_renderer.h"
#include "ui.h"
#include "utility.h"
#include "widget_button.h"
#include "widget_event.h"
#include <cassert>
#include <stdexcept>


namespace DialogSettings {
	constexpr Color colFont = COLOR_WHITE;
	constexpr Color colBorder = COLOR_GREEN;
	constexpr Color colTitleBg = COLOR_CYAN;
	constexpr Color colBodyBg = COLOR_BLACK;
	constexpr Color colBtnBgOut = COLOR_RED;
	constexpr Color colBtnBgOver = COLOR_WHITE;
	constexpr Color colBtnBgDown = COLOR_GREEN;
	constexpr int borderWidth = 2;
	constexpr int titlePadding = 10;
	constexpr int btnPadding = 5;
	constexpr int btnSpacing = 10;
}


static void checkMax(int& max, const int check) {
	if (check > max)
		max = check;
}


Dialog::Dialog() {
	sizePolicy = WidgetSizePolicy::FIXED;
	updateContentBounds();
}


Dialog::~Dialog() {
	SDL::freeNull(texTitle);
	SDL::freeNull(texMessage);
	for (auto btn : buttons)
		delete btn;
}


void Dialog::setData(std::shared_ptr<DialogData> p) {
	using namespace DialogSettings;
	assert(!data);
	assert(p);
	data = p;
	assert(!data->title.empty());
	assert(!data->message.empty());
	assert(!data->buttonText.empty());
	TextRenderer* tr = GameData::instance().resources->getDefaultTR();
	tr->setColor(colFont);
	// setup title
	SDL_Surface* surf = tr->render(data->title);
	copyDim(surf, boundsTitle);
	texTitle = SDL::toTexture(surf);
	boundsTitleBar.x = 0;
	boundsTitleBar.y = 0;
	boundsTitleBar.h = boundsTitle.h + titlePadding;
	boundsTitle.x = boundsTitleBar.x;
	boundsTitle.y = boundsTitleBar.y + ((boundsTitleBar.h - boundsTitle.h) / 2);
	// setup message
	boundsBody.x = boundsTitleBar.x;
	boundsBody.y = boundsTitleBar.y + boundsTitleBar.h;
	surf = tr->render(data->message);
	copyDim(surf, boundsMessage);
	texMessage = SDL::toTexture(surf);
	boundsMessage.x = boundsBody.x;
	boundsMessage.y = boundsBody.y;
	// create buttons
	WidgetState widgetStates[] = {WidgetState::OUT, WidgetState::OVER, WidgetState::DOWN};
	StyledRectangle sRect;
	sRect.setTextRenderer(tr);
	sRect.setTextColor(colFont);
	int x = boundsMessage.x;
	const int y = boundsMessage.y + boundsMessage.h + 10;
	int maxY = y;
	BitmapButton* btn;
	SDL_Texture* tex;
	for (std::size_t i = 0; i < data->buttonText.size(); ++i) {
		btn = new BitmapButton;
		btn->_setIndex(buttons.size());
		btn->_setPos(IntPair{x, y});
		btn->setCallback(std::bind(&Dialog::buttonCallback, this, i));
		sRect.setText(data->buttonText[i]);
		sRect.setSize(
			sRect.getWidth() + btnPadding * 2,
			sRect.getHeight() + btnPadding * 2
		);
		for (std::size_t j = 0; j < (sizeof(widgetStates) / sizeof(widgetStates[0])); ++j) {
			switch (widgetStates[j]) {
			case WidgetState::OUT:
				sRect.setBackgroundColor(colBtnBgOut);
				break;
			case WidgetState::OVER:
				sRect.setBackgroundColor(colBtnBgOver);
				break;
			case WidgetState::DOWN:
				sRect.setBackgroundColor(colBtnBgDown);
				break;
			default:
				assert(false);
			}
			surf = sRect.generate();
			btn->_resize(IntPair{surf->w, surf->h}, WidgetResizeFlag::NONE);
			tex = SDL::toTexture(surf);
			btn->setTexture(widgetStates[j], tex);
		}
		x += btn->getSize().first + btnSpacing;
		checkMax(maxY, btn->getPos().second + btn->getSize().second);
		btn->_setParent(this);
		buttons.push_back(btn);
	}
	int maxX = buttons.back()->getPos().first + buttons.back()->getSize().first;
	checkMax(maxX, boundsTitle.x + boundsTitle.w);
	checkMax(maxX, boundsMessage.x + boundsMessage.w);
	boundsTitleBar.w = maxX + 10;
	boundsBody.w = boundsTitleBar.w;
	boundsBody.h = maxY - boundsTitleBar.y;
	IntPair newSize = IntPair{
		boundsTitleBar.w + (borderWidth * 2),
		boundsTitleBar.h + boundsBody.h + (borderWidth * 2)
	};
	_doResize(newSize);
}


void Dialog::draw(Canvas& can) {
	using namespace DialogSettings;
	can.setColor(colBorder);
	can.fillRect(0, 0, bounds.w, bounds.h);
	can.setRelViewport(contentBounds);
	can.setColor(colTitleBg);
	can.fillRect(boundsTitleBar);
	can.draw(texTitle, &boundsTitle);
	can.setColor(colBodyBg);
	can.fillRect(boundsBody);
	can.draw(texMessage, &boundsMessage);
	for (auto btn : buttons) {
		can.setRelViewport(btn->getBounds());
		btn->draw(can);
		can.clearViewport();
	}
	can.clearViewport();
}


void Dialog::event(WidgetEvent& e) {
	WEMouseAutoUpdate mTest;
	mTest.set(e, bounds);
	for (auto btn : buttons)
		btn->event(e);
}


IntPair Dialog::getPrefSize() const {
	return getSize();
}


IntPair Dialog::getMinSize() const {
	return getSize();
}


void Dialog::_requestResize(Widget*, const IntPair&) {
	assert(false);	// not implemented
}


void Dialog::_resize(const IntPair& p, const WidgetResizeFlag) {
	assert((p.first > 0) && (p.second > 0));
	//! Note: this is not properly implemented and assumes the call was made to itself
	bounds.w = p.first;
	bounds.h = p.second;
	updateContentBounds();
}


void Dialog::buttonCallback(const std::size_t index) {
	if (!isDone) {
		data->selected = index;
		isDone = true;
	}
}


void Dialog::updateContentBounds() {
	using DialogSettings::borderWidth;
	contentBounds.x = borderWidth;
	contentBounds.y = borderWidth;
	contentBounds.w = bounds.w - (borderWidth * 2);
	contentBounds.h = bounds.h - (borderWidth * 2);
}
