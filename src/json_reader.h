#pragma once

#include "sdl_header.h"	// SDL_Rect
#include <rapidjson/document.h>
#include <memory>
#include <string>


namespace JSONHelper {

template<class Iter>	// array iterator
int getIntAndInc(Iter& it) {
	const int n = it->GetInt();
	++it;
	return n;
}


template<class Iter>	// array iterator
void readRect(SDL_Rect& r, Iter& it) {
	r.x = getIntAndInc(it);
	r.y = getIntAndInc(it);
	r.w = getIntAndInc(it);
	r.h = getIntAndInc(it);
}

} // namespace JSONHelper


namespace JSONReader {
	std::shared_ptr<rapidjson::Document> read(const std::string&);	// returns nullptr on error
	std::shared_ptr<rapidjson::Document> read2(const std::string&);	// throws Exception on error
	// Logs exception and exits on failure
	void validateRoom(const rapidjson::Document&, const std::string&);
}
