#include "utility.h"
#include <cmath>	// ceil


int msToTicks(const float frame, const int ms) {
	return std::ceil(static_cast<float>(ms) / frame);
}


bool AlphaFade::update() {
	if (counter.finished()) return true;
	counter.increment();
	if (counter.finished()) {
		alpha = alphaEnd;
	}
	else {
		alphaf += da;
		alpha = static_cast<Uint8>(alphaf);
	}
	return false;
}


void AlphaFade::set(const Uint8 start, const Uint8 end, const unsigned int ticks) {
	alpha = start;
	alphaEnd = end;
	alphaf = start;
	da = -(static_cast<int>(start) - end) / static_cast<float>(ticks);
	counter.reset();
	counter.setMaxTicks(ticks);
}
