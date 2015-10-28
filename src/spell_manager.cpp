#include "spell_manager.h"
#include "attack_manager.h"
#include "canvas.h"
#include "game_data.h"
#include "image.h"
#include "input_handler.h"
#include "main_game_objects.h"
#include "player.h"
#include "sdl_helper.h"
#include "shape_renderer.h"
#include "spell.h"
#include "utility.h"
#include <cassert>
// Spells
#include "spell_basic.h"


namespace SpellManagerSettings {
	constexpr Color colBg = COLOR_BLACK;
	constexpr Color colSpellBasic = COLOR_YELLOW;
}


SpellManager::SpellManager() {
	using namespace SpellManagerSettings;
	SDL_Surface* surfBasic = ShapeRenderer::circle(colSpellBasic, colBg, Constants::SPELL_RENDER_RADIUS);
	SDL::setColorKey(surfBasic, colBg);
	imgBasic = std::make_shared<Image>(surfBasic);
	SDL::freeNull(surfBasic);
}


Spell* SpellManager::newPlayerSpell(const SpellType st) {
	Spell* ret;
	switch (st) {
	case SpellType::BASIC:
		ret = newPlayerSpellBasic();
		break;
	default:
		ret = nullptr;
	}
	return ret;
}


void SpellManager::playerRelease(Spell*& spell) {
	spell->setEndPos(
		GameData::instance().inputHandler->mX(),
		GameData::instance().inputHandler->mY(),
		Constants::SMTravelSpeed
	);
	GameData::instance().mgo->getAttackManager().add(spell);
	spell = nullptr;	// AttackManager has ownership of spell
}


Spell* SpellManager::newPlayerSpellBasic() {
	SpellBasic* s = new SpellBasic{imgBasic.get()};
	s->setSource(&(GameData::instance().mgo->getPlayer()));
	s->setTarget(AttackTarget::CREATURES);
	s->init(1, 15, 18);	//! upgrades not implemented
	return s;
}
