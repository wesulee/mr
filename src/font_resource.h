#pragma once

#include "sdl_header.h"
#include <string>


struct FontResource {
	std::string name;
	TTF_Font* font = nullptr;
	int size = 0;
	bool shared = true;
};
