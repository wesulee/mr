#include "health_bar.h"
#include "canvas.h"
#include "constants.h"
#include "font.h"
#include "game_data.h"
#include "image.h"
#include "resource_manager.h"
#include "ui.h"
#include <cassert>


SDL_Rect CreatureHealthBar::bg = {0, 0, Constants::CHealthWidth, Constants::CHealthHeight};


CreatureHealthBar::CreatureHealthBar(const float hp) : HealthBarBase(hp) {
}


// x is the middle of where bar should be drawn, y is top of bar
void CreatureHealthBar::draw(Canvas& can, const int x, const int y) {
	bg.x = x - Constants::CHealthWidth / 2;
	bg.y = y;
	can.setColor(COLOR_BLACK);
	can.fillRect(bg);
	can.setColor(Color{204, 0, 0});
	can.fillRect(
		bg.x + Constants::CHealthPadX,
		y + Constants::CHealthPadY,
		drawWidth,
		Constants::CHealthHeight - (Constants::CHealthPadY * 2)
	);
}


void CreatureHealthBar::damage(const float d) {
	HealthBarBase::damage(d);
	drawWidth = getRatio() * (Constants::CHealthWidth - Constants::CHealthPadX * 2);
}


PlayerHealthBar::PlayerHealthBar() : HealthBarBase(100) {
	Font font{Font::DEFAULT, 16};
	fr = GameData::instance().resources->loadFont(font);
	Gradient g;		// text
	g.setFront(COLOR_RED);
	g.add(0.2, COLOR_RED);
	g.add(0.5, COLOR_GREEN);
	g.add(0.8, COLOR_WHITE);
	g.setBack(COLOR_WHITE);
	SDL_Surface* textSurf = renderSolidTextGradient(fr, "Health", g);
	text = std::make_shared<Image>(textSurf);
	SDL::freeNull(textSurf);
	textSurf = nullptr;

	Gradient g2;	// health bar
	g2.setFront(COLOR_RED);
	g2.add(0.5, COLOR_YELLOW);
	g2.setBack(Color{0, 170, 0});
	barColor = g2.generate(static_cast<int>(
		Constants::PHealthBarWidth / Constants::PHealthGradRatio + 1
	));

	barX = Constants::PHealthPosX + text->getDrawWidth() + 5;
	barY = (
		Constants::PHealthPosY
		+ (Constants::windowHeight - Constants::PHealthPosY - Constants::PHealthBarHeight) / 2
	);

	updateWidth();
}


PlayerHealthBar::~PlayerHealthBar() {
	GameData::instance().resources->unloadFont(fr);
}


void PlayerHealthBar::draw(Canvas& can) {
	can.draw(*text, Constants::PHealthPosX, Constants::PHealthPosY);
	#ifndef NDEBUG
		auto oldColor = can.getColor();
		auto oldAlpha = can.getAlpha();
		can.setColor(COLOR_RED, SDL_ALPHA_OPAQUE / 3);
		can.fillRect(
			Constants::PHealthPosX, Constants::PHealthPosY,
			text->getDrawWidth(), text->getDrawHeight()
		);
		can.setColor(oldColor, oldAlpha);
	#endif

	can.setColor(barColor[index], SDL_ALPHA_OPAQUE);
	can.fillRect(barX, barY, drawWidth, Constants::PHealthBarHeight);
}


void PlayerHealthBar::damage(const float d) {
	HealthBarBase::damage(d);
	updateWidth();
}


void PlayerHealthBar::updateWidth() {
	drawWidth = static_cast<int>(getRatio() * Constants::PHealthBarWidth);
	index = static_cast<std::size_t>(drawWidth / Constants::PHealthGradRatio);
	assert(index < barColor.size());
}
