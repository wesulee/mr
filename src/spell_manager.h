#pragma once

#include "constants.h"
#include "shapes.h"
#include "spell_type.h"
#include <list>
#include <memory>


class Canvas;
class Image;
class Player;
class Spell;


// Manages creating Spells
class SpellManager {
	SpellManager(const SpellManager&) = delete;
	void operator=(const SpellManager&) = delete;
public:
	SpellManager();
	~SpellManager();
	void update(const Constants::float_type);
	void drawPlayerSpell(Canvas&);
	void selectSpell(const SpellType);
	void press(void);
	void release(void);
	void setPlayer(Player*);
private:
	void updateSpellPos(void);
	Spell* newSpellBasic(void);

	SpellType selected = SpellType::NONE;
	Spell* spell = nullptr;
	std::shared_ptr<Image> spellImg;	// Image associated with spell
	bool activeSpell = false;
	Player* player = nullptr;
	std::shared_ptr<Image> imgBasic;
};
