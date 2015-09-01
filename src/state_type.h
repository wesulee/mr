#pragma once

enum class StateType {NONE, INIT, MENU, GAME, GAME_MENU, LOAD_MENU, SAVE_MENU, DIALOG};


#ifndef NDEBUG
#include <string>


std::string toString(const StateType);

#endif // NDEBUG
