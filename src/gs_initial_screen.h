#pragma once

#include "game_state.h"
#include "utility.h"
#include "widget_area.h"


class ProgressBar;


// The initial GameState, transition to menu after loading initial resources
class InitialScreen : public GameState {
public:
	InitialScreen(std::shared_ptr<StateContext>);
	~InitialScreen();
	void update(void) override;
	void draw(Canvas&) override;
	void entered(void) override;
	void leaving(const StateType, std::shared_ptr<StateContext>) override;
	void obscuring(const StateType, std::shared_ptr<StateContext>) override;
	void revealed(std::shared_ptr<StateContext>) override;
private:
	WidgetArea wArea;
	Counter counter;
	ProgressBar* bar;
};
