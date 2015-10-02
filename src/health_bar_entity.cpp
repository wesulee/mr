#include "health_bar_entity.h"
#include "canvas.h"
#include "constants.h"
#include "entity.h"
#include "game_data.h"


namespace EntityHealthBarSettings {
	constexpr uint32_t drawDuration = 5000;	// duration bar is drawn after entity damaged
	constexpr int bgWidth = 22;
	constexpr int bgHeight = 5;
	constexpr int padFill = 1;
	constexpr int padBounds = 1;
	constexpr Color colBg = COLOR_BLACK;
	constexpr Color colBar = Color{204, 0, 0};
}


EntityHealthBar::EntityHealthBar(const KillableGameEntity* e, const int hp) : HealthBar(e, hp) {
}


// Bar is drawn centered horizontally and just below entity bounds
void EntityHealthBar::draw(Canvas& can) {
	using namespace EntityHealthBarSettings;
	if (drawTest) {
		drawTest = ((GameData::instance().time - damageTime) < drawDuration);
		if (drawTest) {
			SDL_Rect entityBounds = entity->getBounds();
			SDL_Rect dst;
			dst.w = bgWidth;
			dst.h = bgHeight;
			dst.x = (entityBounds.x + ((entityBounds.w - dst.w) / 2));
			dst.y = (entityBounds.y + entityBounds.h + padBounds);
			can.setColor(colBg);
			can.fillRect(dst);
			dst.x += padFill;
			dst.y += padFill;
			dst.h -= (padFill * 2);
			dst.w = barWidth;
			can.setColor(colBar);
			can.fillRect(dst);
		}
	}
}


void EntityHealthBar::set(const int hp) {
	using namespace EntityHealthBarSettings;
	health = hp;
	damageTime = GameData::instance().time;
	drawTest = true;
	barWidth = static_cast<int>(getRatio() * (bgWidth - (padFill * 2)));
}
