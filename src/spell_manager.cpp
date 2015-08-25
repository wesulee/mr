#include "spell_manager.h"
#include "attack_manager.h"
#include "canvas.h"
#include "constants.h"
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


SpellManager::SpellManager() {
	Spell::setFadeTicks(static_cast<unsigned int>(msToTicks(Constants::frameDurationFloat, Constants::SMFadeDurMS)));

	Color colBg = COLOR_BLACK;
	Color colBasic = COLOR_YELLOW;
	SDL_Surface* surfBasic = ShapeRenderer::circle(colBasic, colBg, Constants::SPELL_RENDER_RADIUS);
	SDL_SetColorKey(surfBasic, SDL_TRUE, SDL::mapRGB(surfBasic->format, colBg));
	imgBasic = std::make_shared<Image>(surfBasic);
	SDL::freeNull(surfBasic);
	SpellBasic::setImage(imgBasic.get());

	selectSpell(SpellType::BASIC);	// default
}


SpellManager::~SpellManager() {
	if (spell != nullptr) {
		delete spell;
		spell = nullptr;
	}
}


void SpellManager::update() {
	if (activeSpell)
		spell->chargeTick();
}


// Because drawing of Player's spell should be done immediately after
// drawing Player and Player having no knowledge of how to draw spells,
// Player must request SpellManager to draw active Spell
void SpellManager::drawPlayerSpell(Canvas& can) {
	assert(player != nullptr);
	if (!activeSpell) return;
	assert(spell != nullptr);
	assert(spellImg);
	updateSpellPos();
	spellImg->setAlpha(SDL_ALPHA_OPAQUE);
	spell->draw(can);
}


void SpellManager::selectSpell(const SpellType st) {
	selected = st;
	switch(selected) {
	case SpellType::NONE:
		spellImg = nullptr;
		break;
	case SpellType::BASIC:
		spellImg = imgBasic;
		break;
	}
}


void SpellManager::press() {
	if (activeSpell) return;
	assert(spell == nullptr);
	switch (selected) {
	case SpellType::NONE:
		return;
	case SpellType::BASIC:
		spell = newSpellBasic();
		break;
	}
	activeSpell = true;
}


void SpellManager::release() {
	if (activeSpell) {
		updateSpellPos();
		spell->setEndPos(
			GameData::instance().inputHandler->mX(),
			GameData::instance().inputHandler->mY(),
			Constants::SMTravelSpeed
		);
		spell->setSource(player);
		spell->setTarget(AttackTarget::CREATURES);
		GameData::instance().mgo->getAttackManager().add(spell);
		spell = nullptr;	// AttackManager has ownership of spell
		activeSpell = false;
	}
}


void SpellManager::setPlayer(Player* p) {
	assert(p != nullptr);
	player = p;
}


void SpellManager::updateSpellPos() {
	assert(spell != nullptr);
	SDL_Rect r = player->getBounds();
	spell->setPosX(r.x + r.w / 2);
	spell->setPosY(r.y - spell->getRadius() + Constants::SMPlayerOffset);
}


Spell* SpellManager::newSpellBasic() {
	SpellBasic* s = new SpellBasic;
	s->init(1, 15, 0.3);	//! upgrades not implemented
	return s;
}
