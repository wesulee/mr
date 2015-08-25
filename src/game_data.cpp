#include "game_data.h"
#include <boost/filesystem.hpp>
#include <cstdlib>
#define BOOST_FILESYSTEM_NO_DEPRECATED


static void checkPath(std::string& s) {
	if (!s.empty() && (s.back() != boost::filesystem::path::preferred_separator)) {
		s += boost::filesystem::path::preferred_separator;
		s.shrink_to_fit();
	}
}


GameData::GameData() : randGen(seed()), exitCode(EXIT_SUCCESS) {
}


void GameData::setDataPath(const std::string& s) {
	dataPath = s;
	checkPath(dataPath);
}


void GameData::setSavePath(const std::string& s) {
	savePath = s;
	checkPath(savePath);
}


std::random_device::result_type GameData::seed() {
	std::random_device rd;
	return rd();
}
