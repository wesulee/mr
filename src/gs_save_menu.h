#pragma once

#include "game_state.h"
#include "widget_area.h"


class TextItem;
class TextEdit;


class SaveMenu : public GameState {
	typedef SaveMenu self_type;
public:
	SaveMenu(std::shared_ptr<StateContext>);
	~SaveMenu();
	void update(void) override;
	void draw(Canvas&) override;
	void entered(void) override;
	void leaving(const StateType, std::shared_ptr<StateContext>) override;
	void obscuring(const StateType, std::shared_ptr<StateContext>) override;
	void revealed(std::shared_ptr<StateContext>) override;
private:
	void selectedCallback(TextItem*);
	void saveButtonCallback(void);
	void enableWidgetEvents(void);
	void disableWidgetEvents(void);

	WidgetArea wArea;
	TextEdit* textEdit;
};
