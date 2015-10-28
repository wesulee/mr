#include "player.h"
#include "canvas.h"
#include "entity_resource.h"
#include "game_data.h"
#include "main_game_objects.h"
#include "resource_manager.h"
#include "room.h"
#include "save_data.h"
#include "sdl_header.h"
#include "spell.h"
#include "spell_manager.h"
#include "sprite_sheet.h"
#include <cassert>
#include <cmath>


namespace PlayerSettings {
	constexpr int spellOffsetY = -8;
}


class PlayerResource : public EntityResource {
public:
	PlayerResource();
	~PlayerResource();

	SpriteSheet* ss;
};


PlayerResource::PlayerResource() : EntityResource(EntityResourceID::PLAYER) {
	ss = GameData::instance().resources->getSpriteSheet("default", false, true);
}


PlayerResource::~PlayerResource() {
	GameData::instance().resources->freeSpriteSheet("default", false, true);
}


// in order to draw Player, must add Sprites through MultistateSprite::addState()
Player::Player()
: KillableGameEntity(Constants::PHealth), healthBar(this, Constants::PHealth)
, speed(Constants::PBaseMovSpeed) {
	entityPos.x = 200;	// default values
	entityPos.y = 200;
	EntityResource* res = GameData::instance().resources->getEntity(this, EntityResourceID::PLAYER);
	PlayerResource* pRes = dynamic_cast<PlayerResource*>(res);
	ms.addState(pRes->ss->get("player_l"));
	ms.addState(pRes->ss->get("player_r"));
}


Player::~Player() {
	if (spell != nullptr)
		delete spell;
	GameData::instance().resources->freeEntity(this, EntityResourceID::PLAYER);
}


bool Player::update(const Constants::float_type dt) {
	const Constants::float_type delta = (speed * dt);
	if (moving) {
		switch (direction) {
		case PlayerDirection::NONE:
			break;
		case PlayerDirection::N:
			move(0, -delta);
			break;
		case PlayerDirection::E:
			move(delta, 0);
			ms.setState(SPR_STATE_RIGHT);
			break;
		case PlayerDirection::S:
			move(0, delta);
			break;
		case PlayerDirection::W:
			move(-delta, 0);
			ms.setState(SPR_STATE_LEFT);
			break;
		case PlayerDirection::NE:
			move(delta, -delta);
			ms.setState(SPR_STATE_RIGHT);
			break;
		case PlayerDirection::SE:
			move(delta, delta);
			ms.setState(SPR_STATE_RIGHT);
			break;
		case PlayerDirection::SW:
			move(-delta, delta);
			ms.setState(SPR_STATE_LEFT);
			break;
		case PlayerDirection::NW:
			move(-delta, -delta);
			ms.setState(SPR_STATE_LEFT);
			break;
		default:
			break;
		}
	}
	if (spell != nullptr) {
		spell->chargeTick(dt);
		updateSpellPos();
	}
	return false;
}


void Player::draw(Canvas& can) {
	healthBar.draw(can);
	can.draw(ms, static_cast<int>(entityPos.x), static_cast<int>(entityPos.y));
	if (spell != nullptr)
		spell->draw(can);
}


void Player::mousePress() {
	if (spell == nullptr) {
		spell = GameData::instance().mgo->getSpellManager().newPlayerSpell(spellType);
		updateSpellPos();
	}
}


void Player::mouseRelease() {
	if (spell != nullptr) {
		GameData::instance().mgo->getSpellManager().playerRelease(spell);
		assert(spell == nullptr);
	}
}


EntityResource* Player::loadResource() {
	return new PlayerResource;
}


void Player::unloadResource(EntityResource* res) {
	delete res;
}


SDL_Rect Player::getBounds() const {
	return {static_cast<int>(entityPos.x), static_cast<int>(entityPos.y), ms.getDrawWidth(), ms.getDrawHeight()};
}


void Player::damage(const int d) {
	decHealth(d);
	healthBar.set(entityHealth);
}


void Player::setDirection(const int dir) {
	switch (dir) {
	case PLAYER_DIR_NONE:
	case PLAYER_DIR_N | PLAYER_DIR_S:
	case PLAYER_DIR_E | PLAYER_DIR_W:
	case PLAYER_DIR_N | PLAYER_DIR_S | PLAYER_DIR_E| PLAYER_DIR_W:
		moving = false;
		direction = PlayerDirection::NONE;
		break;
	case PLAYER_DIR_N:
	case PLAYER_DIR_N | PLAYER_DIR_E | PLAYER_DIR_W:
		moving = true;
		direction = PlayerDirection::N;
		break;
	case PLAYER_DIR_N | PLAYER_DIR_E:
		moving = true;
		direction = PlayerDirection::NE;
		break;
	case PLAYER_DIR_E:
	case PLAYER_DIR_E | PLAYER_DIR_N | PLAYER_DIR_S:
		moving = true;
		direction = PlayerDirection::E;
		break;
	case PLAYER_DIR_E | PLAYER_DIR_S:
		moving = true;
		direction = PlayerDirection::SE;
		break;
	case PLAYER_DIR_S:
	case PLAYER_DIR_S | PLAYER_DIR_E | PLAYER_DIR_W:
		moving = true;
		direction = PlayerDirection::S;
		break;
	case PLAYER_DIR_S | PLAYER_DIR_W:
		moving = true;
		direction = PlayerDirection::SW;
		break;
	case PLAYER_DIR_W:
	case PLAYER_DIR_W | PLAYER_DIR_N | PLAYER_DIR_S:
		moving = true;
		direction = PlayerDirection::W;
		break;
	case PLAYER_DIR_N | PLAYER_DIR_W:
		moving = true;
		direction = PlayerDirection::NW;
		break;
	default:
		assert(false);	// given dir is invalid
	}
}


void Player::setSaveData(SaveData& data) const {
	data.posX = entityPos.x;
	data.posY = entityPos.y;
	data.health = entityHealth;
}


void Player::getSaveData(const SaveData& data) {
	entityPos.x = data.posX;
	entityPos.y = data.posY;
	entityHealth = data.health;
	healthBar.refresh();
}


void Player::move(const Constants::float_type dx, const Constants::float_type dy) {
	 GameData::instance().mgo->getRoom().update(*this, Vector2D<>{dx, dy});
	 // update spell pos in update()
}


void Player::updateSpellPos() {
	assert(spell != nullptr);
	spell->setPos(getBounds(), PlayerSettings::spellOffsetY);
}
