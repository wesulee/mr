#ifndef NDEBUG
#include "state_type.h"
#include <cassert>


std::string toString(const StateType st) {
	std::string str;
	switch(st) {
	case StateType::NONE:
		str = "NONE";
		break;
	case StateType::INIT:
		str = "INIT";
		break;
	case StateType::MENU:
		str = "MENU";
		break;
	case StateType::GAME:
		str = "GAME";
		break;
	case StateType::GAME_MENU:
		str = "GAME_MENU";
		break;
	case StateType::LOAD_MENU:
		str = "LOAD_MENU";
		break;
	case StateType::SAVE_MENU:
		str = "SAVE_MENU";
		break;
	case StateType::DIALOG:
		str = "DIALOG";
		break;
	default:
		assert(false);
	}
	return str;
}

#endif // NDEBUG
