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
		// pos should end at integers.
		// Make sure that when pos is casted to int, it is correct.
		if (static_cast<float>(static_cast<int>(pos.x)) - pos.x < 0)
			pos.x += 0.1f;
		if (static_cast<float>(static_cast<int>(pos.y)) - pos.y < 0)
			pos.y += 0.1f;
		return true;
	}
	else {
		pos += dpos;
		counter.increment();
		return false;
	}
}


void Spell::setPosX(const int x) {
	pos.x = x;
}


void Spell::setPosY(const int y) {
	pos.y = y;
}


// speed is pixels per second
void Spell::setEndPos(const int x, const int y, const float speed) {
	fade->setOffset(x, y);	// needs to be updated in subclass
	const float dist = std::sqrt(
		square(static_cast<float>(x) - pos.x)
		+ square(static_cast<float>(y) - pos.y)
	);
	const float ms = dist / speed * 1000;
	const int ticks = static_cast<int>(std::ceil(ms / Constants::frameDurationFloat));
	dpos.x = (static_cast<float>(x) - pos.x) / ticks;
	dpos.y = (static_cast<float>(y) - pos.y) / ticks;
	counter.setMaxTicks(static_cast<unsigned int>(ticks));
	counter.reset();
}


void Spell::setFadeTicks(const unsigned int n) {
	fadeTicks = n;
}
