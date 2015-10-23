#include "spell.h"
#include "utility.h"
#include "vfx_fade.h"
#include <cassert>
#include <cmath>	// sqrt


Spell::Spell() : fade(new VFXFade) {
	fade->setFade(Constants::SMFadeDur, SDL_ALPHA_OPAQUE, SDL_ALPHA_TRANSPARENT);
}


Spell::~Spell() {
	if (fade != nullptr) {
		delete fade;
		fade = nullptr;
	}
}


bool Spell::update(const Constants::float_type dt) {
	if ((timeRem - dt) <= 0) {
		pos += (vel * timeRem);
		pos.x = correctFloat(pos.x);
		pos.y = correctFloat(pos.y);
		timeRem = 0;
		return true;
	}
	else {
		pos += (vel * dt);
		timeRem -= dt;
		return false;
	}}


void Spell::setPosX(const int x) {
	pos.x = static_cast<Constants::float_type>(x);
}


void Spell::setPosY(const int y) {
	pos.y = static_cast<Constants::float_type>(y);
}


// this assumes that current position is integer
// speed is distance per second
void Spell::setEndPos(const int x, const int y, const Constants::float_type speed) {
	fade->setOffset(x, y);	// needs to be updated in subclass
	Vector2D<> deltaPos{
		static_cast<Constants::float_type>(x - static_cast<int>(pos.x)),
		static_cast<Constants::float_type>(y - static_cast<int>(pos.y))
	};
	timeRem = deltaPos.length() / speed;
	if (timeRem == 0) {
		vel = Vector2D<>{0, 0};
	}
	else {
		vel = deltaPos / timeRem;
	}
}
