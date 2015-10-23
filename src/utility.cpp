#include "utility.h"
#include <cmath>	// ceil


int msToTicks(const float frame, const int ms) {
	return std::ceil(static_cast<float>(ms) / frame);
}


bool AlphaFade::update(const Constants::float_type dt) {
	if ((timeRem - dt) <= 0) {
		assert(timeRem != 0);	// update() called after it returned true before
		// add floatInc to make sure correct alpha when casted to Uint8
		alpha += (dAlpha * timeRem) + Constants::floatInc;
		timeRem = 0;
		return true;
	}
	else {
		alpha += (dAlpha * dt);
		timeRem -= dt;
		return false;
	}
}


void AlphaFade::set(const Constants::float_type dur, const Uint8 alphaBeg, const Uint8 alphaEnd) {
	alpha = static_cast<Constants::float_type>(alphaBeg);
	dAlpha = (
		(static_cast<Constants::float_type>(alphaEnd) - static_cast<Constants::float_type>(alphaBeg))
		/ dur
	);
	timeRem = dur;
}
