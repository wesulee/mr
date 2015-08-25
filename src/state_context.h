#pragma once

#include "state_type.h"
#include <map>
#include <memory>
#include <string>


class Image;


struct StateContext {
	// the GameState that generated this object
	std::map<int, int> mIntInt;
	std::map<int, std::string> mIntStr;
	std::map<std::string, std::shared_ptr<Image>> images;
	StateType sourceType = StateType::NONE;	// the type that generated context
};
