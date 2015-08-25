#pragma once

#include "game_state.h"
#include "widget_area.h"


class TextItem;


class LoadMenu : public GameState {
	typedef LoadMenu self_type;
public:
	LoadMenu(std::shared_ptr<StateContext>);
	~LoadMenu();
	void update(void) override;
	void draw(Canvas&) override;
	void entered(void) override;
	void leaving(const StateType, std::shared_ptr<StateContext>) override;
	void obscuring(const StateType, std::shared_ptr<StateContext>) override;
	void revealed(std::shared_ptr<StateContext>) override;
private:
	void event(WidgetEvent&);
	void selectedCallback(TextItem*);
	void enableWidgetEvents(void);
	void disableWidgetEvents(void);

	WidgetArea wArea;
	TextItem* selected = nullptr;
};
