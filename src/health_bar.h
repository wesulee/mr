#pragma once

#include "color.h"
#include "font_resource.h"
#include "sdl_helper.h"
#include <algorithm>
#include <memory>
#include <vector>


class Canvas;
class Image;


class HealthBarBase {
public:
	HealthBarBase(const float hp) : health(hp), maxHealth(hp) {}
	virtual ~HealthBarBase() = default;
	float getHealth() const {return health;}
	float getMaxHealth() const {return maxHealth;}
	virtual void damage(const float d) {health = std::max(health - d, 0.0f);}
	bool isAlive() const {return health > 0;}
	float getRatio() const {return health / maxHealth;}
private:
	float health;
	float maxHealth;
};


class CreatureHealthBar : public HealthBarBase {
public:
	CreatureHealthBar(const float);
	~CreatureHealthBar() = default;
	void draw(Canvas&, const int, const int);
	void damage(const float) override;
private:
	int drawWidth = 0;
	static SDL_Rect bg;
};


class PlayerHealthBar : public HealthBarBase {
public:
	PlayerHealthBar();
	~PlayerHealthBar();
	void draw(Canvas&);
	void damage(const float) override;
private:
	void updateWidth(void);

	FontResource fr;
	std::vector<Color> barColor;
	std::shared_ptr<Image> text;
	std::size_t index = 0;
	int barX;
	int barY;
	int drawWidth = 0;
};
