#pragma once


class Canvas;
class HealthBar;


class GameInterface {
public:
	GameInterface() = default;
	~GameInterface();
	void init(void);
	void draw(Canvas&);
private:
	HealthBar* playerHealthBar;
};
