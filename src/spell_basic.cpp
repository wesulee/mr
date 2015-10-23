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


SpellBasic::SpellBasic() : radius(5), dradius(0), radiusLimit(5) {
}


SpellBasic::~SpellBasic() {
	// do nothing
}


bool SpellBasic::update(const Constants::float_type dt) {
	if (Spell::update(dt)) {
		const int fadeRad = static_cast<int>(radius * Constants::SMFadeRadMult);
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
				getRadius()
			},
			static_cast<int>(15.0f * radius / static_cast<Constants::float_type>(radiusLimit))	//! upgrades not implemented
		);
		return true;
	}
	return false;
}


void SpellBasic::draw(Canvas& can) {
	assert(img != nullptr);
	const int radiusInt = getRadius();
	img->setWidth(radiusInt * 2);
	img->setHeight(radiusInt * 2);
	can.draw(*img, static_cast<int>(pos.x) - radiusInt, static_cast<int>(pos.y) - radiusInt);
}


// initial radius, maximum radius size, radius growth per second
void SpellBasic::init(const int radInit, const int radLim, const Constants::float_type drad) {
	assert(radInit >= 0);
	assert(radLim >= radInit);
	assert(drad >= 0);
	radius = static_cast<Constants::float_type>(radInit);
	dradius = drad;
	radiusLimit = radLim;
}


void SpellBasic::chargeTick(const Constants::float_type dt) {
	if (static_cast<int>(radius) < radiusLimit) {
		radius += (dradius * dt);
		const int radiusInt = static_cast<int>(radius);
		if (radiusInt > radiusLimit)
			radius = radiusLimit;
	}
}


int SpellBasic::getRadius() const {
	return static_cast<int>(radius);
}


void SpellBasic::setImage(Image* i) {
	assert(i != nullptr);
	img = i;
}
