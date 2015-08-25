#pragma once

#include "spell.h"


class Image;


// Usage: after constructor, call init(). chargeTick() while charging.
// Once released, pass to AttackManager
class SpellBasic : public Spell {
public:
	SpellBasic();
	~SpellBasic();
	bool update(void) override;
	void draw(Canvas&) override;
	void init(const int, const int, const float);
	void chargeTick(void) override;
	int getRadius(void) const override;
	static void setImage(Image*);
private:
	static Image* img;
	int radius;
	float radiusf;
	int radiusLimit;
	float dr;	// change in radius per update
};
