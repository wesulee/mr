#pragma once

#include "attack.h"
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
	void setPosX(const int);
	void setPosY(const int);
	void setEndPos(const int, const int, const Constants::float_type);
	virtual int getRadius(void) const = 0;
protected:
	Vector2D<> pos;
	Vector2D<> vel;	// velocity
	float timeRem;
	VFXFade* fade;
};
