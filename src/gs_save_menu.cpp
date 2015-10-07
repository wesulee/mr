#include "gs_save_menu.h"
#include "canvas.h"
#include "constants.h"
#include "dialog_data.h"
#include "game_data.h"
#include "input_handler.h"
#include "main_game_objects.h"
#include "menu_shared.h"
#include "resource_manager.h"
#include "save_helper.h"
#include "state_context.h"
#include "state_manager.h"
#include "utility.h"	// q
#include "widget_button.h"
#include "widget_event.h"
#include "widget_layout.h"
#include "widget_text_edit.h"
#include "widget_text_item.h"
#include "widget_text_list_view.h"


namespace SaveMenuSettings {
	constexpr Color bgColor = COLOR_BLACK;
	constexpr Color colTEText = COLOR_BLACK;
	constexpr Color colTEBg = COLOR_WHITE;
	constexpr Color colTEOutline = COLOR_GREEN;
	constexpr Color colBtnText = COLOR_BLACK;
	constexpr Color colBtnOutline = COLOR_GREEN;
	constexpr Color colBtnBgOut = COLOR_WHITE;
	constexpr Color colBtnBgOver = COLOR_CYAN;
	constexpr Color colBtnBgDown = COLOR_GREEN;
	constexpr int paddingWidget = 10;
	constexpr int btnOutlineSize = 1;
	constexpr int btnTextPadH = 1;
	constexpr int btnTextPadV = 1;
	constexpr int btnSpacing = 10;
}


// duplicate code in main_menu.cpp
template<class Layout, class Style>
static void addButton(Layout layout, Style style, const char* str, ButtonCallback func) {
	TextButton* btn = new TextButton;
	btn->setStyle(style);
	btn->setText(str);
	btn->setCallback(func);
	layout->add(btn);
}


SaveMenu::SaveMenu(std::shared_ptr<StateContext> sc) : GameState(StateType::SAVE_MENU, sc) {
	using namespace SaveMenuSettings;
	using std::placeholders::_1;
	getCallbacks()->setKey(SDLK_ESCAPE, CommonCallback::popStateK);
	getCallbacks()->setDefaultKey(DefaultCallback::key);
	getCallbacks()->setMouse(DefaultCallback::mouse);
	getCallbacks()->setEvent(DefaultCallback::event);
	// setup widgets
	VerticalLayout* const layoutV = new VerticalLayout;
	layoutV->setMargins(0, 30, 0, 0);
	layoutV->setSpacing(paddingWidget);
	layoutV->setWidgetAlignment(WidgetAlignmentHoriz::RIGHT, WidgetAlignmentVert::TOP);
	TextListView* const view = new TextListView;
	view->setSelectedCallback(std::bind(&self_type::selectedCallback, this, _1));
	MenuSettings::setup(*view);
	textEdit = new TextEdit;
	textEdit->setStyle(colTEText, colTEBg, colTEOutline);
	// setup buttons
	std::shared_ptr<TextButton::Style> style = std::make_shared<TextButton::Style>();
	style->tr = GameData::instance().resources->getDefaultTR();
	style->outlineSz = btnOutlineSize;
	style->padTextH = btnTextPadH;
	style->padTextV = btnTextPadV;
	style->colText = colBtnText;
	style->colOutline = colBtnOutline;
	style->colBgOut = colBtnBgOut;
	style->colBgOver = colBtnBgOver;
	style->colBgDown = colBtnBgDown;
	HorizontalLayout* layoutH = new HorizontalLayout;
	layoutH->setMargins(5, 0, 0, btnSpacing);
	layoutH->setWidgetAlignment(WidgetAlignmentHoriz::RIGHT, WidgetAlignmentVert::TOP);
	layoutH->setSpacing(btnSpacing);
	addButton(layoutH, style, "Save", std::bind(&self_type::saveButtonCallback, this));
	addButton(layoutH, style, "Cancel", CommonCallback::popState);
	layoutV->add(view);
	layoutV->add(textEdit);
	layoutV->add(layoutH);
	wArea.setPosition(IntPair{0, 0});
	wArea.setSize(IntPair{Constants::windowWidth, Constants::windowHeight});
	wArea.setLayout(layoutV);
}


SaveMenu::~SaveMenu() {
	// don't delete textEdit
}


void SaveMenu::update() {
}


void SaveMenu::draw(Canvas& can) {
	can.setColor(SaveMenuSettings::bgColor, SDL_ALPHA_OPAQUE);
	can.clearScreen();
	wArea.draw(can);
}


void SaveMenu::entered() {
	enableWidgetEvents();
}


void SaveMenu::leaving(const StateType, std::shared_ptr<StateContext>) {
	disableWidgetEvents();
}


void SaveMenu::obscuring(const StateType, std::shared_ptr<StateContext>) {
	disableWidgetEvents();
}


void SaveMenu::revealed(std::shared_ptr<StateContext> sc) {
	enableWidgetEvents();
	if (sc->sourceType == StateType::DIALOG) {
		const std::size_t i = GameData::instance().wData.dialogData->selected;
		GameData::instance().wData.dialogData = nullptr;
		if (i == 0) {	// yes to overwrite
			GameData::instance().mgo->saveGame(textEdit->getText());
			GameData::instance().stateManager->pop();
		}
	}
}


void SaveMenu::selectedCallback(TextItem* item) {
	if (item != nullptr) {
		textEdit->setText(item->getText());
	}
}


void SaveMenu::saveButtonCallback() {
	if (textEdit->getText().empty())
		return;
	if (SaveHelper::userSaveExists(textEdit->getText())) {
		// setup dialog
		assert(!GameData::instance().wData.dialogData);
		std::shared_ptr<DialogData> data = std::make_shared<DialogData>();
		GameData::instance().wData.dialogData = data;
		data->title = "Confirm Save Overwrite";
		data->message =
			"The save file " + q(textEdit->getText()) + " already exists. Are you sure you want to overwrite?"
		;
		data->buttonText.reserve(2);
		data->buttonText.push_back("Yes");
		data->buttonText.push_back("No");
		GameData::instance().stateManager->push(StateType::DIALOG);
	}
	else {
		GameData::instance().mgo->saveGame(textEdit->getText());
		GameData::instance().stateManager->pop();
	}
}


void SaveMenu::enableWidgetEvents() {
	using std::placeholders::_1;
	InputHandler* const ih = GameData::instance().inputHandler;
	ih->setWidgetEvents(true);
	ih->setWidgetEventCallback(std::bind(&WidgetArea::event, &wArea, _1));
}


void SaveMenu::disableWidgetEvents() {
	GameData::instance().inputHandler->setWidgetEvents(false);
}
