#pragma once

#include "spell.h"


class Image;


// Usage: after constructor, call init(). chargeTick() while charging.
// Once released, pass to AttackManager
class SpellBasic : public Spell {
public:
	SpellBasic(Image*);
	~SpellBasic() {/* do nothing */}
	bool update(const Constants::float_type) override;
	void draw(Canvas&) override;
	void init(const int, const int, const Constants::float_type);
	void chargeTick(const Constants::float_type) override;
private:
	Image* image;
	Constants::float_type dradius;	// radius growth per second
	int radiusLimit;
};
