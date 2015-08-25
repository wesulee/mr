#pragma once

#include "attack.h"
#include "sdl_header.h"


class AttackRect : public Attack {
public:
	AttackRect();
	AttackRect(const SDL_Rect&, const float);
	~AttackRect() = default;
	bool update() override;
	void draw(Canvas&) override;
	void setRect(const SDL_Rect&);
	void setDamage(const float);
private:
	SDL_Rect rect;
	float damage;
};
