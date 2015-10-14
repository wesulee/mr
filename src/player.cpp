#include "player.h"
#include "canvas.h"
#include "constants.h"
#include "entity_resource.h"
#include "game_data.h"
#include "resource_manager.h"
#include "room.h"
#include "save_data.h"
#include "sprite_sheet.h"
#include <cassert>
#include <cmath>


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
, speed(Constants::PBaseMovSpeed * Constants::frameDurationFloat / 1000) {
	entityPos.x = 200;	// default values
	entityPos.y = 200;
	EntityResource* res = GameData::instance().resources->getEntity(this, EntityResourceID::PLAYER);
	PlayerResource* pRes = dynamic_cast<PlayerResource*>(res);
	ms.addState(pRes->ss->get("player_l"));
	ms.addState(pRes->ss->get("player_r"));
}


Player::~Player() {
	GameData::instance().resources->freeEntity(this, EntityResourceID::PLAYER);
}


bool Player::update() {
	if (moving) {
		switch (direction) {
		case PlayerDirection::NONE:
			break;
		case PlayerDirection::N:
			move(0, -speed);
			break;
		case PlayerDirection::E:
			move(speed, 0);
			ms.setState(SPR_STATE_RIGHT);
			break;
		case PlayerDirection::S:
			move(0, speed);
			break;
		case PlayerDirection::W:
			move(-speed, 0);
			ms.setState(SPR_STATE_LEFT);
			break;
		case PlayerDirection::NE:
			move(speed, -speed);
			ms.setState(SPR_STATE_RIGHT);
			break;
		case PlayerDirection::SE:
			move(speed, speed);
			ms.setState(SPR_STATE_RIGHT);
			break;
		case PlayerDirection::SW:
			move(-speed, speed);
			ms.setState(SPR_STATE_LEFT);
			break;
		case PlayerDirection::NW:
			move(-speed, -speed);
			ms.setState(SPR_STATE_LEFT);
			break;
		default:
			break;
		}
	}
	return false;
}


void Player::draw(Canvas& can) {
	healthBar.draw(can);
	can.draw(ms, static_cast<int>(entityPos.x), static_cast<int>(entityPos.y));
}


void Player::setRoom(Room* r) {
	room = r;
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


void Player::updatePos(const float dx, const float dy) {
	entityPos.x += dx;
	entityPos.y += dy;
}


void Player::setPos(const float x, const float y) {
	entityPos.x = x;
	entityPos.y = y;
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


void Player::move(const float dx, const float dy) {
	int newXInt;
	int newYInt;
	float newX = entityPos.x + dx;
	float newY = entityPos.y + dy;
	if (dx >= 0) {
		newXInt = static_cast<int>(std::ceil(newX));
	}
	else {
		newXInt = static_cast<int>(newX);
	}
	if (dy >= 0) {
		newYInt = static_cast<int>(std::ceil(newY));
	}
	else {
		newYInt = static_cast<int>(newY);
	}
	if (room->space(newXInt, newYInt, ms.getDrawWidth(), ms.getDrawHeight())) {
		entityPos.x = newX;
		entityPos.y = newY;
	}
	else {
		room->updateEntity(*this, newXInt, newYInt);
	}
}
