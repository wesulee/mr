#pragma once

#include "game_state.h"
#include "widget_area.h"


class Dialog;


/*
Before DialogState can be constructed, must setup WidgetData::dialogData.
Previous gamestate should push this state.
Once a button has been selected, DialogData::selected is updated, and this state is popped.
After original gamestate restored, DialogData should be destroyed.
*/
class DialogState : public GameState {
public:
	DialogState(std::shared_ptr<StateContext>);
	~DialogState();
	void update(const Constants::float_type) override;
	void draw(Canvas&) override;
	void entered(void) override;
	void leaving(const StateType, std::shared_ptr<StateContext>) override;
	void obscuring(const StateType, std::shared_ptr<StateContext>) override;
	void revealed(std::shared_ptr<StateContext>) override;
private:
	void enableWidgetEvents(void);
	void disableWidgetEvents(void);

	WidgetArea wArea;
	Dialog* dialog;
};
