#include "spell_basic.h"
#include "attack_manager.h"
#include "canvas.h"
#include "game_data.h"
#include "image.h"
#include "main_game_objects.h"
#include "shapes.h"
#include "vfx_fade.h"
#include "vfx_manager.h"
#include <cassert>


Image* SpellBasic::img = nullptr;


SpellBasic::SpellBasic() : radius(1), radiusf(radius), radiusLimit(radiusf), dr(0.0) {
}


SpellBasic::~SpellBasic() {
	// do nothing
}


bool SpellBasic::update() {
	radiusf += dr;
	radius = static_cast<int>(radiusf);
	if (Spell::update()) {
		const int fadeRad = static_cast<int>(radius * Spell::fadeRadMult);
		fade->setImage(img, fadeRad * 2, fadeRad * 2);
		// center the fade
		fade->setOffset(
			fade->getOffsetX() - fadeRad,
			fade->getOffsetY() - fadeRad
		);
		GameData::instance().mgo->getVFXManager().add(fade);
		fade = nullptr;	// VFXManager now owns fade
		GameData::instance().mgo->getAttackManager().procCirc(
			this,
			Circle{
				static_cast<int>(pos.x),
				static_cast<int>(pos.y),
				radius
			},
			15.0f * radiusf / static_cast<float>(radiusLimit)	//! upgrades not implemented
		);
		return true;
	}
	else
		return false;
}


void SpellBasic::draw(Canvas& can) {
	assert(img != nullptr);
	img->setWidth(radius * 2);
	img->setHeight(radius * 2);
	const int x = static_cast<int>(pos.x) - radius;
	const int y = static_cast<int>(pos.y) - radius;
	can.draw(*img, x, y);
}


// initial radius, maximum radius size, radius growth per tick
void SpellBasic::init(const int rad, const int radLim, const float drad) {
	assert(rad >= 0);
	assert(radLim >= 0);
	assert(drad >= 0);
	radius = rad;
	radiusf = radius;
	radiusLimit = radLim;
	dr = drad;
}


void SpellBasic::chargeTick() {
	if (radius != radiusLimit) {
		radiusf += dr;
		radius = static_cast<int>(radiusf);
		if (radius > radiusLimit)
			radius = radiusLimit;
	}
}


int SpellBasic::getRadius() const {
	return radius;
}


void SpellBasic::setImage(Image* i) {
	assert(i != nullptr);
	img = i;
}
