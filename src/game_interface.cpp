#include "game_interface.h"
#include "canvas.h"
#include "game_data.h"
#include "main_game_objects.h"
#include "player.h"


GameInterface::~GameInterface() {
	// do nothing
}


void GameInterface::init() {
	playerHealthBar = GameData::instance().mgo->getPlayer().getHealthBar();
}


void GameInterface::draw(Canvas& can) {
	playerHealthBar->draw(can);
}
