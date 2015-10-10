#include "save_helper.h"
#include "constants.h"
#include "exception.h"
#include "game_data.h"
#include "logger.h"
#include "save_data.h"
#include "utility.h"	// q
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/vector.hpp>
#include <fstream>
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
	for (
		auto it = fs::directory_iterator{GameData::instance().savePath}, end = fs::directory_iterator{};
		it != end;
		++it
	) {
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
	std::string filePath = getPath(userNameToFileName(name));
	fs::file_status status = fs::status(filePath);
	if (fs::exists(status)) {
		// move old save to temporary path, save new data, remove old save
		std::string tempFilePath = getPath(getTempFileName(userNameToFileName(name)));
		fs::file_status statusTemp = fs::status(tempFilePath);
		if (fs::exists(statusTemp)) {
			// temporary file path already exists, report error
			Logger::instance().log(RuntimeError{
				"unable to save",
				"temp path " + q(tempFilePath) + " already exists"
			});
			return;
		}
		fs::rename(filePath, tempFilePath);
		doSave(filePath, data);
		fs::remove(tempFilePath);
	}
	else {
		doSave(filePath, data);
	}
}


std::shared_ptr<SaveData> SaveHelper::getData(const std::string& name) {
	std::string filePath = getPath(userNameToFileName(name));
	std::shared_ptr<SaveData> data = std::make_shared<SaveData>();
	std::ifstream file{filePath};
	boost::archive::text_iarchive ar{file};
	ar & (*data);
	return data;
}


std::string SaveHelper::getTempFileName(const std::string& fname) {
	return (fname + ".tmp");
}


std::string SaveHelper::getPath(const std::string& fname) {
	return (GameData::instance().savePath + fname);
}


void SaveHelper::doSave(const std::string& filePath, const SaveData& data) {
	std::ofstream file{filePath};
	boost::archive::text_oarchive ar{file};
	ar & data;
}
