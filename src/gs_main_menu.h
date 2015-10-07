#pragma once

#include "game_state.h"
#include "sdl_helper.h"
#include "widget_area.h"


class MainMenu : public GameState {
public:
	MainMenu(std::shared_ptr<StateContext>);
	~MainMenu();
	void update(void) override;
	void draw(Canvas&) override;
	void entered(void) override;
	void leaving(const StateType, std::shared_ptr<StateContext>) override;
	void obscuring(const StateType, std::shared_ptr<StateContext>) override;
	void revealed(std::shared_ptr<StateContext>) override;
private:
	void enableWidgetEvents(void);
	void disableWidgetEvents(void);

	WidgetArea wArea;
	SDL_Rect titleBounds;
	SDL_Texture* texTitle;
};
