#include "json_reader.h"
#include "constants.h"
#include "exception.h"
#include "game_data.h"
#include "json_data.h"
#include "logger.h"
#include "room_inc.h"
#include "sdl_header.h"
#include <rapidjson/document.h>
#include <rapidjson/filereadstream.h>
#include <rapidjson/error/en.h>
#include <cassert>
#include <cstdio>


namespace JSONHelper {

// returns path if it can't do it
// also adds quotes
static std::string getRelPathToDataDir(const std::string& filePath) {
	std::size_t i = filePath.find(GameData::instance().dataPath);
	if (i != 0)
		return (std::string{'\"'} + filePath + '\"');
	return (std::string{'\"'} + filePath.substr(GameData::instance().dataPath.size()) + '\"');
}


static void read(const std::string& filePath, rapidjson::Document& doc) {
#if defined(DEBUG_JSON_READ) && DEBUG_JSON_READ
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "READ " << getRelPathToDataDir(filePath) << std::endl;
#endif
#if defined(_WIN32)
	FILE* f = std::fopen(filePath.c_str() , "rb");
#else
	FILE* f = std::fopen(filePath.c_str() , "r");
#endif // _WIN32
	if (f == nullptr) {
		std::perror("fopen");
		throw FileError{filePath, FileError::Err::NOT_OPEN};
	}
	char buffer[Constants::JSONBufferSz];
	rapidjson::FileReadStream is{f, buffer, sizeof(buffer)};
	doc.ParseStream(is);
	std::fclose(f);
	if (doc.HasParseError()) {
		ParserError error{ParserError::DataType::JSON};
		error.setPath(filePath);
		error.setOffset(doc.GetErrorOffset());
		error.setWhat("error parsing json", rapidjson::GetParseError_En(doc.GetParseError()));
		throw error;
	}
}


template<class T>
static int getIntAndInc(T& it) {
	const int n = it->GetInt();
	++it;
	return n;
}


template<class T>
static void readRect(SDL_Rect& r, T& it) {
	r.x = getIntAndInc(it);
	r.y = getIntAndInc(it);
	r.w = getIntAndInc(it);
	r.h = getIntAndInc(it);
}


template<class T>	// T is array
void procRoomConnecting(const T& array, std::vector<std::pair<int, int>>& vec) {
	namespace rj = rapidjson;
	vec.reserve(array.Size() / 2);
	// process one pair at a time
	std::pair<int, int> p;
	for (rj::Value::ConstValueIterator it = array.Begin(); it != array.End();) {
		p.first = getIntAndInc(it);
		p.second = getIntAndInc(it);
		vec.push_back(p);
	}
}

} // namespace JSONHelper


std::shared_ptr<SpriteSheetData> JSONReader::loadSpriteSheet(const std::string& filePath) {
	std::shared_ptr<SpriteSheetData> data;
	try {
		data = runLoadSpriteSheet(filePath);
	}
	catch (Exception const& e) {
		Logger::instance().log(e);
	}
	return data;
}


std::shared_ptr<RoomData> JSONReader::loadRoom(const std::string& filePath) {
	std::shared_ptr<RoomData> data;
	try {
		data = runLoadRoom(filePath);
	}
	catch (Exception const& e) {
		Logger::instance().log(e);
	}
	return data;
}


std::shared_ptr<CreatureData> JSONReader::loadCreature(const std::string& filePath) {
	std::shared_ptr<CreatureData> data;
	try {
		data = runLoadCreature(filePath);
	}
	catch (Exception const& e) {
		Logger::instance().log(e);
	}
	return data;
}


std::shared_ptr<SpriteSheetData> JSONReader::runLoadSpriteSheet(const std::string& filePath) {
	using namespace JSONHelper;
	namespace rj = rapidjson;
	rj::Document doc;
	std::shared_ptr<SpriteSheetData> data = std::make_shared<SpriteSheetData>();
	read(filePath, doc);
#if defined(DEBUG_JSON_PROC) && DEBUG_JSON_PROC
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "PROC spritesheet " << getRelPathToDataDir(filePath) << std::endl;
#endif
	data->image = doc["img"].GetString();
	{	// process sprites
		SDL_Rect tmpRect;
		std::string tmpStr;
		const rj::Value& sprites = doc["sprites"];
		for (rj::Value::ConstValueIterator it = sprites.Begin(); it != sprites.End(); ++it) {
			rj::Value::ConstValueIterator it2 = it->Begin();
			tmpStr = it2->GetString();
			++it2;
			readRect(tmpRect, it2);
			data->sprites.emplace(tmpStr, tmpRect);
		}
	}
	return data;
}


std::shared_ptr<RoomData> JSONReader::runLoadRoom(const std::string& filePath) {
	using namespace JSONHelper;
	namespace rj = rapidjson;
	rj::Document doc;
	std::shared_ptr<RoomData> data = std::make_shared<RoomData>();
	read(filePath, doc);
#if defined(DEBUG_JSON_PROC) && DEBUG_JSON_PROC
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "PROC room " << getRelPathToDataDir(filePath) << std::endl;
#endif
	{	// process background
		RoomSpriteData tmpRS;
		const rj::Value& background = doc["background"];
		for (rj::Value::ConstValueIterator it = background.Begin(); it != background.End(); ++it) {
			tmpRS.name = (*it)["name"].GetString();
			tmpRS.x = (*it)["x"].GetInt();
			tmpRS.y = (*it)["y"].GetInt();
			data->bg.push_back(tmpRS);
		}
	}
	{	// process block
		SDL_Rect tmpRect;
		const rj::Value& block = doc["block"];
		for (rj::Value::ConstValueIterator it = block.Begin(); it != block.End(); ++it) {
			rj::Value::ConstValueIterator it2 = it->Begin();
			readRect(tmpRect, it2);
			data->block.push_back(tmpRect);
		}
	}
	{	// process creatures
		RoomCreatureData tmpCD;
		const rj::Value& creatures = doc["creatures"];
		for (rj::Value::ConstValueIterator it = creatures.Begin(); it != creatures.End(); ++it) {
			tmpCD.name = (*it)["name"].GetString();
			tmpCD.x = (*it)["x"].GetInt();
			tmpCD.y = (*it)["y"].GetInt();
			data->creatures.push_back(tmpCD);
		}
	}
	{	// process conn
		const rj::Value& conn = doc["conn"];
		procRoomConnecting(conn["n"], data->connecting[SideToIndex(Side::NORTH)]);
		procRoomConnecting(conn["e"], data->connecting[SideToIndex(Side::EAST)]);
		procRoomConnecting(conn["s"], data->connecting[SideToIndex(Side::SOUTH)]);
		procRoomConnecting(conn["w"], data->connecting[SideToIndex(Side::WEST)]);
	}
	return data;
}


std::shared_ptr<CreatureData> JSONReader::runLoadCreature(const std::string& filePath) {
	using namespace JSONHelper;
	namespace rj = rapidjson;
	rj::Document doc;
	std::shared_ptr<CreatureData> data = std::make_shared<CreatureData>();
	read(filePath, doc);
#if defined(DEBUG_JSON_PROC) && DEBUG_JSON_PROC
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "PROC creature " << getRelPathToDataDir(filePath) << std::endl;
#endif
	{	// process attributes
		const rj::Value& attributes = doc["attr"];
		for (rj::Value::ConstMemberIterator it = attributes.MemberBegin(); it != attributes.MemberEnd(); ++it) {
			data->attributes[it->name.GetString()] = std::to_string(it->value.GetInt());
		}
	}

	// process animations
	if (doc.HasMember("animations")) {
		const rj::Value& animations = doc["animations"];
		for (rj::Value::ConstValueIterator it = animations.Begin(); it != animations.End(); ++it) {
			data->animations.emplace_front();
			auto& obj = data->animations.front();
			for (rj::Value::ConstMemberIterator it2 = it->MemberBegin(); it2 != it->MemberEnd(); ++it2) {
				if (it2->value.IsInt())
					obj[it2->name.GetString()] = std::to_string(it2->value.GetInt());
				else
					obj[it2->name.GetString()] = it2->value.GetString();
			}
		}
	}
	//! TODO process the rest
	return data;
}
