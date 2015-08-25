#pragma once

#include "creature_spawner.h"
#include "sdl_header.h"
#include "sprite.h"


class Creature1Sp : public CreatureSpawner {
public:
	Creature1Sp();
	~Creature1Sp() = default;
	void spawn(CreatureManager*, const int, const int) override;
	bool update(void) override;
	void draw(Canvas&) override;
	SDL_Rect getBounds(void) const override;
private:
	Sprite sp;
};
