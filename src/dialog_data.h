#pragma once

#include "constants.h"
#include <cstddef>
#include <string>
#include <vector>


struct DialogData {
	std::string title;
	std::string message;
	std::vector<std::string> buttonText;
	std::size_t selected = Constants::maxIndex;
};
