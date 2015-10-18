#pragma once

#include "attack.h"
#include "utility.h"
#include "utility_struct.h"


// NOTE: because VFXFade offset is top-left coordinate,
// needs to be set again by Spell subclass after released to properly center.
class VFXFade;


class Spell : public Attack {
public:
	Spell();
	~Spell();
	bool update(void) override;
	virtual void chargeTick(void) = 0;
	void setPosX(const int);
	void setPosY(const int);
	void setEndPos(const int, const int, const float);
	virtual int getRadius(void) const = 0;
	static void setFadeTicks(const unsigned int);
protected:
	Vector2D<float> pos;
	Vector2D<float> dpos;	// position delta
	Counter counter;
	VFXFade* fade;
	static unsigned int fadeTicks;
	static constexpr float fadeRadMult = 1.6;	// fadeout radius multiplier
};
