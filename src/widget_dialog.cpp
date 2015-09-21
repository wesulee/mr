#include "widget_dialog.h"
#include "canvas.h"
#include "constants.h"
#include "dialog_data.h"
#include "game_data.h"
#include "resource_manager.h"
#include "text_renderer.h"
#include "ui.h"
#include "utility.h"
#include "widget_button.h"
#include "widget_event.h"
#include "widget_text.h"
#include <cassert>
#include <stdexcept>


namespace DialogSettings {
	constexpr int borderSz = 2;
	constexpr int widgetSpacing = 10;	// vertical layout spacing
	constexpr int btnSpacing = 10;
	constexpr int minWidth = static_cast<int>(Constants::windowWidth * 0.5);
	constexpr Color colBg = COLOR_BLACK;
	constexpr Color colBorder = COLOR_GREEN;
	constexpr Color colText = COLOR_WHITE;
	constexpr Color colTitleBg = COLOR_CYAN;
	constexpr Color colBtnText = COLOR_WHITE;
	constexpr Color colBtnBgOut = COLOR_RED;
	constexpr Color colBtnBgOver = COLOR_GREEN;
	constexpr Color colBtnBgDown = COLOR_CYAN;
}


Dialog::Dialog(std::shared_ptr<DialogData> p) {
	using namespace DialogSettings;
	sizePolicy = WidgetSizePolicy::PREFER;
	layout._setPos(IntPair{borderSz, borderSz});
	layout.setMargins(0, 0, 0, 0);
	layout.setWidgetAlignment(WidgetAlignmentHoriz::LEFT, WidgetAlignmentVert::TOP);
	layout.setSpacing(widgetSpacing);
	// process data
	assert(p);
	data = p;
	assert(!data->title.empty());
	assert(!data->message.empty());
	assert(!data->buttonText.empty());
	TextRenderer* tr = GameData::instance().resources->getDefaultTR();
	WidgetText* textTitle = new WidgetText;
	textTitle->enableBackground(colTitleBg);
	textTitle->setRenderer(tr);
	textTitle->setTextColor(colText);
	textTitle->setText(data->title);
	WidgetText* textMessage = new WidgetText;
	textMessage->setRenderer(tr);
	textMessage->setTextColor(colText);
	textMessage->setText(data->message);
	// add buttons
	HorizontalLayout* hLayout = new HorizontalLayout;
	hLayout->setWidgetAlignment(WidgetAlignmentHoriz::LEFT, WidgetAlignmentVert::TOP);
	hLayout->setSpacing(btnSpacing);
	std::shared_ptr<TextButton::Style> buttonStyle = std::make_shared<TextButton::Style>();
	buttonStyle->tr = tr;
	buttonStyle->outlineSz = 0;
	buttonStyle->colText = colBtnText;
	buttonStyle->colBgOut = colBtnBgOut;
	buttonStyle->colBgOver = colBtnBgOver;
	buttonStyle->colBgDown = colBtnBgDown;
	TextButton* button;
	for (std::size_t i = 0; i < data->buttonText.size(); ++i) {
		button = new TextButton;
		button->setStyle(buttonStyle);
		button->setText(data->buttonText[i]);
		button->setCallback(std::bind(&self_type::buttonCallback, this, i));
		hLayout->add(button);
	}
	// add to layout
	layout.add(textTitle);
	layout.add(textMessage);
	layout.add(hLayout);
	// finalize
	layout._setParent(this);
	_resize(getPrefSize(), WidgetResizeFlag::SELF);
}


Dialog::~Dialog() {
}


void Dialog::draw(Canvas& can) {
	using namespace DialogSettings;
	SDL_Rect r{0, 0, bounds.w, bounds.h};
	if (borderSz > 0) {
		can.setColor(colBorder);
		can.fillRect(r);
		shrinkRect(r, borderSz);
		can.setColor(colBg);
		can.fillRect(r);
		can.setRelViewport(r);
		layout.draw(can);
		can.clearViewport();
	}
	else {
		can.setColor(colBg);
		can.fillRect(r);
		layout.draw(can);
	}
}


void Dialog::event(WidgetEvent& e) {
	WEMouseAutoUpdate mTest;
	mTest.set(e, bounds);
	layout.event(e);
}


IntPair Dialog::getPrefSize() const {
	IntPair p;
	// the preferred width is larger of minWidth and horizontal layout's width (which contains the buttons)
	p.first = std::max(
		DialogSettings::minWidth,
		layout.get(2)->getPrefSize().first
	);
	p.second = layout.getPrefSize().second;
	p.first += (DialogSettings::borderSz * 2);
	p.second += (DialogSettings::borderSz * 2);
	return p;
}


IntPair Dialog::getMinSize() const {
	IntPair p = layout.getMinSize();
	p.first += (DialogSettings::borderSz * 2);
	p.second += (DialogSettings::borderSz * 2);
	return p;
}


void Dialog::_requestResize(Widget* w, const IntPair&) {
	assert(w == static_cast<Widget*>(&layout));
	(void)w;
	// Do not resize child.
}


void Dialog::_resize(const IntPair& p, const WidgetResizeFlag f) {
	using namespace DialogSettings;
	assert((p.first > 0) && (p.second > 0));
	bounds.w = p.first;
	bounds.h = p.second;
	switch (f) {
	case WidgetResizeFlag::SELF:
	case WidgetResizeFlag::PARENT:
		layout._resize(
			IntPair{bounds.w - (borderSz * 2), bounds.h - (borderSz * 2)},
			WidgetResizeFlag::PARENT
		);
		break;
	default:
		break;
	}
}


void Dialog::buttonCallback(const std::size_t index) {
	if (!isDone) {
		data->selected = index;
		isDone = true;
	}
}
