#pragma once

#include "attack.h"
#include "sdl_helper.h"		// SDL_Rect
#include "utility_struct.h"


// NOTE: because VFXFade offset is top-left coordinate,
// needs to be set again by Spell subclass after released to properly center.
class VFXFade;


class Spell : public Attack {
public:
	Spell();
	~Spell();
	bool update(const Constants::float_type) override;
	virtual void chargeTick(const Constants::float_type) = 0;
	void setPos(const SDL_Rect&, const int);
	void setEndPos(const int, const int, const Constants::float_type);
	int getRadius(void) const;
protected:
	Vector2D<> pos;
	Vector2D<> vel;	// velocity
	VFXFade* fade;
	Constants::float_type timeRem;
	Constants::float_type radius;
};


inline
int Spell::getRadius() const {
	return static_cast<int>(radius);
}
