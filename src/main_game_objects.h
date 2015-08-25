#pragma once

#include <functional>
#include <string>


class AttackManager;
class CreatureManager;
class Player;
class Room;
class SpellManager;
class VFXManager;


class MainGameObjects {
	friend class MainGame;
public:
	MainGameObjects() = default;
	~MainGameObjects() {}
	// provide a way to call MainGame::save()
	void saveGame(const std::string& fname) {saveFunction(fname);}
	AttackManager& getAttackManager(){return *_AttackManager;}
	CreatureManager& getCreatureManager() {return *_CreatureManager;}
	Player& getPlayer() {return *_Player;}
	Room& getRoom() {return *_Room;}
	SpellManager& getSpellManager() {return *_SpellManager;}
	VFXManager& getVFXManager() {return *_VFXManager;}
private:
	std::function<void(const std::string&)> saveFunction;
	AttackManager* _AttackManager = nullptr;
	CreatureManager* _CreatureManager = nullptr;
	Player* _Player = nullptr;
	Room* _Room = nullptr;
	SpellManager* _SpellManager = nullptr;
	VFXManager* _VFXManager = nullptr;
};
