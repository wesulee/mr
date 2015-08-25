#include "save_helper.h"
#include "constants.h"
#include "game_data.h"
#include "resource_manager.h"
#include <boost/filesystem.hpp>
#define BOOST_FILESYSTEM_NO_DEPRECATED


namespace fs = boost::filesystem;


bool SaveHelper::userSaveExists(const std::string& name) {
	std::string filePath = GameData::instance().savePath + userNameToFileName(name);
	fs::file_status status = fs::status(filePath);
	return fs::exists(status);
}


std::vector<std::string> SaveHelper::getUserSaveNames() {
	std::vector<std::string> names;
	fs::path ext{std::string{'.'} + Constants::saveFileExt};
	for (auto it = fs::directory_iterator{GameData::instance().savePath}, end = fs::directory_iterator{}; it != end; ++it) {
		if (fs::is_regular_file(*it) && (it->path().extension() == ext)) {
			names.push_back((it->path().stem().string()));
		}
	}
	return names;
}


// just adds the save file extension to save name
std::string SaveHelper::userNameToFileName(const std::string& name) {
	return (name + '.' + Constants::saveFileExt);
}


void SaveHelper::save(const std::string& name, const SaveData& data) {
	GameData::instance().resources->saveSaveData(userNameToFileName(name), data);
}


std::shared_ptr<SaveData> SaveHelper::getData(const std::string& name) {
	return GameData::instance().resources->getSaveData(userNameToFileName(name));
}
