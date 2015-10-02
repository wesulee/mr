#include "health_bar_player.h"
#include "canvas.h"
#include "constants.h"
#include "font.h"
#include "game_data.h"
#include "image.h"
#include "resource_manager.h"
#include "sdl_helper.h"
#include "ui.h"
#include <cassert>


PlayerHealthBar::PlayerHealthBar(const KillableGameEntity* e, const int hp) : HealthBar(e, hp) {
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
		(Constants::PHealthBarWidth / Constants::PHealthGradRatio) + 1
	));

	barX = Constants::PHealthPosX + text->getDrawWidth() + 5;
	barY = (
		Constants::PHealthPosY
		+ ((Constants::windowHeight - Constants::PHealthPosY - Constants::PHealthBarHeight) / 2)
	);

	updateWidth();
}


PlayerHealthBar::~PlayerHealthBar() {
	GameData::instance().resources->unloadFont(fr);
}


void PlayerHealthBar::draw(Canvas& can) {
	can.draw(*text, Constants::PHealthPosX, Constants::PHealthPosY);
#ifndef NDEBUG
	auto oldColor = can.getColorState();
	can.setColor(COLOR_RED, SDL_ALPHA_OPAQUE / 3);
	can.fillRect(
		Constants::PHealthPosX, Constants::PHealthPosY,
		text->getDrawWidth(), text->getDrawHeight()
	);
	can.setColorState(oldColor);
#endif // NDEBUG
	can.setColor(barColor[index]);
	can.fillRect(barX, barY, barWidth, Constants::PHealthBarHeight);
}


void PlayerHealthBar::set(const int hp) {
	health = hp;
	updateWidth();
}


void PlayerHealthBar::updateWidth() {
	barWidth = static_cast<int>(getRatio() * Constants::PHealthBarWidth);
	index = static_cast<std::size_t>(barWidth / Constants::PHealthGradRatio);
	assert(index < barColor.size());
}
