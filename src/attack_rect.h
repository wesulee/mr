#pragma once

#include "attack.h"
#include "sdl_header.h"


class AttackRect : public Attack {
public:
	AttackRect();
	AttackRect(const SDL_Rect&, const int);
	~AttackRect() = default;
	bool update(const Constants::float_type) override;
	void draw(Canvas&) override;
	void setRect(const SDL_Rect&);
	void setDamage(const int);
private:
	SDL_Rect rect;
	int damage;
};
