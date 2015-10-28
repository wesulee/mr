#pragma once

#include "constants.h"
#include "shapes.h"
#include "spell_type.h"
#include <list>
#include <memory>


class Canvas;
class Image;
class Spell;


// Manages creating Spells
class SpellManager {
	SpellManager(const SpellManager&) = delete;
	void operator=(const SpellManager&) = delete;
public:
	SpellManager();
	~SpellManager() = default;
	Spell* newPlayerSpell(const SpellType);
	void playerRelease(Spell*&);
private:
	Spell* newPlayerSpellBasic(void);

	std::shared_ptr<Image> imgBasic;
};
