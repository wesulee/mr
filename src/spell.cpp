#include "spell.h"
#include "constants.h"
#include "vfx_fade.h"
#include <cmath>	// sqrt


unsigned int Spell::fadeTicks = 1;	// default value, set in SpellManager


Spell::Spell() : fade(new VFXFade) {
	fade->setFade(SDL_ALPHA_OPAQUE, SDL_ALPHA_TRANSPARENT, fadeTicks);
}


Spell::~Spell() {
	if (fade != nullptr) {
		delete fade;
		fade = nullptr;
	}
}


bool Spell::update() {
	if (counter.finished()) {
		pos.x = correctFloat(pos.x);
		pos.y = correctFloat(pos.y);
		return true;
	}
	else {
		pos += dpos;
		counter.increment();
		return false;
	}
}


void Spell::setPosX(const int x) {
	pos.x = static_cast<Constants::float_type>(x);
}


void Spell::setPosY(const int y) {
	pos.y = static_cast<Constants::float_type>(y);
}


// speed is pixels per second
void Spell::setEndPos(const int x, const int y, const Constants::float_type speed) {
	fade->setOffset(x, y);	// needs to be updated in subclass
	const Constants::float_type dist = std::sqrt(
		square(static_cast<Constants::float_type>(x) - pos.x)
		+ square(static_cast<Constants::float_type>(y) - pos.y)
	);
	const Constants::float_type ms = dist / speed * 1000;
	const int ticks = static_cast<int>(std::ceil(ms / Constants::frameDurationFloat));
	dpos.x = (static_cast<Constants::float_type>(x) - pos.x) / ticks;
	dpos.y = (static_cast<Constants::float_type>(y) - pos.y) / ticks;
	counter.setMaxTicks(static_cast<unsigned int>(ticks));
	counter.reset();
}


void Spell::setFadeTicks(const unsigned int n) {
	fadeTicks = n;
}
