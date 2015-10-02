#pragma once

#include "color.h"
#include "health_bar.h"
#include "font_resource.h"
#include <memory>
#include <vector>


class Image;


class PlayerHealthBar : public HealthBar {
public:
	PlayerHealthBar(const KillableGameEntity*, const int);
	~PlayerHealthBar();
	void draw(Canvas&) override;
	void set(const int) override;
private:
	void updateWidth(void);

	FontResource fr;
	std::vector<Color> barColor;
	std::shared_ptr<Image> text;
	std::size_t index = 0;
	int barX;
	int barY;
	int barWidth = 0;
};
