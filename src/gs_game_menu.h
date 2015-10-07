#pragma once

#include "game_state.h"
#include "widget_area.h"
#include <memory>


class Image;


class GameMenu : public GameState {
	typedef GameMenu self_type;
public:
	GameMenu(std::shared_ptr<StateContext>);
	~GameMenu();
	void update(void) override;
	void draw(Canvas&) override;
	void entered(void) override;
	void leaving(const StateType, std::shared_ptr<StateContext>) override;
	void obscuring(const StateType, std::shared_ptr<StateContext>) override;
	void revealed(std::shared_ptr<StateContext>) override;
private:
	void saveCallback(void);
	void enableWidgetEvents(void);
	void disableWidgetEvents(void);

	WidgetArea wArea;
	std::shared_ptr<Image> background;
};
