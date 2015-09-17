#include "main_game.h"
#include "canvas.h"
#include "game_data.h"
#include "image.h"
#include "input_handler.h"
#include "parameters.h"
#include "resource_manager.h"
#include "save_data.h"
#include "save_helper.h"
#include "state_context.h"
#include "state_manager.h"
#include <cassert>
#include <memory>


class RoomData;


MainGame::MainGame(std::shared_ptr<StateContext> sc) : GameState(StateType::GAME, sc), cm(&player, &room) {
	using std::placeholders::_1;
	playerDirKeys[0] = SDLK_w;
	playerDirKeys[1] = SDLK_d;
	playerDirKeys[2] = SDLK_s;
	playerDirKeys[3] = SDLK_a;

	getCallbacks()->setKey(SDLK_ESCAPE, CommonCallback::popStateK);
	getCallbacks()->setKey(SDLK_p, std::bind(CommonCallback::pushStateK, _1, StateType::GAME_MENU));
	getCallbacks()->setKey(playerDirKeys[0], std::bind(&self_type::playerDirCallback, this, _1, PLAYER_DIR_N));
	getCallbacks()->setKey(playerDirKeys[1], std::bind(&self_type::playerDirCallback, this, _1, PLAYER_DIR_E));
	getCallbacks()->setKey(playerDirKeys[2], std::bind(&self_type::playerDirCallback, this, _1, PLAYER_DIR_S));
	getCallbacks()->setKey(playerDirKeys[3], std::bind(&self_type::playerDirCallback, this, _1, PLAYER_DIR_W));
	getCallbacks()->setDefaultKey(DefaultCallback::key);
	getCallbacks()->setEvent(std::bind(&self_type::eventCallback, this, _1));

	objects.saveFunction = std::bind(&self_type::save, this, _1);
	objects._AttackManager = &am;
	objects._CreatureManager = &cm;
	objects._Player = &player;
	objects._Room = &room;
	objects._SpellManager = &sm;
	objects._VFXManager = &vfxm;
	GameData::instance().mgo = &objects;

	player.setRoom(&room);
	sm.setPlayer(&player);
	am.setCreatureManager(&cm);

	// check if new game
	if (sc->mIntInt.count(Parameters::NEW_GAME)) {
		setRunningFunc();
		map.setCur(0, 0);
		map.refresh();
		std::shared_ptr<RoomData> roomData = GameData::instance().resources->getRoomData(0, 0);
		room.set(*roomData);
		cm.loadRoom(roomData);
	}
	else {
		setClearedFunc();
		assert(sc->mIntStr.count(Parameters::LOAD_GAME));
		load(sc->mIntStr.at(Parameters::LOAD_GAME));
	}
}


MainGame::~MainGame() {
	GameData::instance().mgo = nullptr;
}


void MainGame::update() {
	(this->*updateFunc)();
}


void MainGame::draw(Canvas& can) {
	can.setColor(COLOR_BLACK, SDL_ALPHA_OPAQUE);
	can.clearScreen();
	room.draw(can);
	player.draw(can);
	sm.drawPlayerSpell(can);
	(this->*drawFunc)(can);
}


void MainGame::entered() {
	refreshPlayerDir();
}


void MainGame::leaving(const StateType, std::shared_ptr<StateContext>) {
}


void MainGame::obscuring(const StateType st, std::shared_ptr<StateContext> sc) {
	sc->mIntInt[Parameters::MAIN_GAME_IS_RUNNING] = !map.isCleared(map.getCurX(), map.getCurY());
	switch (st) {
	case StateType::GAME_MENU:
		obscureToMenu(sc);
		break;
	default:
		break;
	}
}


void MainGame::revealed(std::shared_ptr<StateContext>) {
	refreshPlayerDir();
	map.notifyMoved();
}


void MainGame::updateRunning() {
	player.setDirection(playerDirection);
	player.update();
	sm.update();
	cm.update();
	am.update();
	vfxm.update();
}


void MainGame::updateCleared() {
	player.setDirection(playerDirection);
	player.update();
	sm.update();
	am.update();
	vfxm.update();
}


void MainGame::drawRunning(Canvas& can) {
	cm.draw(can);
	am.draw(can);
	vfxm.draw(can);
}


void MainGame::drawCleared(Canvas& can) {
	am.draw(can);
	vfxm.draw(can);
	map.draw(can);
}


void MainGame::obscureToMenu(std::shared_ptr<StateContext> sc) {
	// generate background image
	SDL_Surface* surf = SDL::copyScreen();
	SDL_Texture* tex = SDL::toTexture(surf);
	sc->images.emplace("background", std::make_shared<Image>(tex));
}


void MainGame::mouseCallbackRunning(const MouseEventType e) {
	switch (e) {
	case MouseEventType::PRESSED:
		sm.press();
		break;
	case MouseEventType::RELEASED:
		sm.release();
		break;
	case MouseEventType::MOVED:
		break;
	}
}


void MainGame::mouseCallbackCleared(const MouseEventType e) {
	switch (e) {
	case MouseEventType::PRESSED:
		sm.press();
		break;
	case MouseEventType::RELEASED:
		sm.release();
		break;
	case MouseEventType::MOVED:
		map.notifyMoved();
		break;
	}
}


void MainGame::playerDirCallback(const KeyEvent& e, const int dir) {
	if (e.pressed)
		playerDirection |= dir;		// set
	else
		playerDirection &= ~dir;	// unset
}


void MainGame::eventCallback(const SDL_Event& e) {
	using std::placeholders::_1;
	switch (e.type) {
	case SDL_WINDOWEVENT:
		if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
			if (GameData::instance().settings.test(GameSettings::Index::PAUSEFOCUSLOST))
				GameData::instance().stateManager->push(StateType::GAME_MENU);
		}
		break;
	default:
		if (e.type == SDL::userEventType) {
			if (e.user.code == Parameters::CREATURES_EMPTY) {
				setClearedFunc();
				map.setClear(map.getCurX(), map.getCurY());
				map.notifyMoved();
				room.notifyClear();
			}
		}
	}
}


// after returning from a state change, keys need to rechecked
void MainGame::refreshPlayerDir() {
	playerDirection = PLAYER_DIR_NONE;
	InputHandler* const ih = GameData::instance().inputHandler;
	if (ih->isPressed(playerDirKeys[0]))
		playerDirection |= PLAYER_DIR_N;
	if (ih->isPressed(playerDirKeys[1]))
		playerDirection |= PLAYER_DIR_E;
	if (ih->isPressed(playerDirKeys[2]))
		playerDirection |= PLAYER_DIR_S;
	if (ih->isPressed(playerDirKeys[3]))
		playerDirection |= PLAYER_DIR_W;
	player.setDirection(playerDirection);
}


void MainGame::setRunningFunc() {
	using std::placeholders::_1;
	getCallbacks()->setMouse(std::bind(&self_type::mouseCallbackRunning, this, _1));
	updateFunc = &self_type::updateRunning;
	drawFunc = &self_type::drawRunning;
}


void MainGame::setClearedFunc() {
	using std::placeholders::_1;
	getCallbacks()->setMouse(std::bind(&self_type::mouseCallbackCleared, this, _1));
	updateFunc = &self_type::updateCleared;
	drawFunc = &self_type::drawCleared;
}


void MainGame::load(const std::string& name) {
	std::shared_ptr<SaveData> data = SaveHelper::getData(name);
	map.getSaveData(*data);
	player.getSaveData(*data);
	auto roomData = GameData::instance().resources->getRoomData(data->roomX, data->roomY);
	room.set(*roomData);
	room.notifyClear();
}


// save current state
void MainGame::save(const std::string& name) {
	assert(map.isCleared(map.getCurX(), map.getCurY()));
	SaveData data;
	map.setSaveData(data);
	player.setSaveData(data);
	SaveHelper::save(name, data);
}
