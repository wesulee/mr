#include "json_reader.h"
#include "data_validation.h"
#include "exception.h"
#include "game_data.h"
#include "json_data.h"
#include "logger.h"
#include "room_inc.h"
#include "sdl_header.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cassert>
#include <vector>
#ifndef NDEBUG
#include "constants.h"
#endif


#ifndef NDEBUG
#define VALID_ARRAY(iter1,iter2) validArray(iter1, iter2)
#else
#define VALID_ARRAY(iter1,iter2) true
#endif // NDEBUG


template<class T>	// T iterator
static bool validArray(T begin, const T end) {
	while (begin != end) {
		if (!begin->first.empty())
			return false;
		++begin;
	}
	return true;
}


namespace JSONHelper {

// returns path if it can't do it
// also adds quotes
static std::string getRelPathToDataDir(const std::string& filePath) {
	std::size_t i = filePath.find(GameData::instance().dataPath);
	if (i != 0)
		return (std::string{'\"'} + filePath + '\"');
	return (std::string{'\"'} + filePath.substr(GameData::instance().dataPath.size()) + '\"');
}


void read(const std::string& filePath, boost::property_tree::ptree& pt) {
	using namespace boost::property_tree;
	try {
#if defined(DEBUG_JSON_READ) && DEBUG_JSON_READ
		DEBUG_BEGIN << DEBUG_JSON_PREPEND << "READ " << getRelPathToDataDir(filePath) << std::endl;
#endif
		read_json(filePath, pt);
	}
	catch (ptree_bad_path const& e) {
		throw FileError{filePath, FileError::Err::MISSING, e.what()};
	}
	catch (json_parser::json_parser_error const& e) {
		ParserError error{ParserError::DataType::JSON};
		error.setPath(filePath);
		error.setWhat(e.message());
		error.setLine(e.line());
		throw error;
	}
}


void checkExists(const boost::property_tree::ptree& pt, const std::vector<std::string>& check, BadData& error) {
	for (const auto& str : check) {
		if (pt.count(str) == 0) {
			error.setDetails("missing \"" + str + '\"');
			throw error;
		}
	}
}


void procRoomConnecting(std::vector<std::pair<int, int>>& pairs,
	const boost::property_tree::ptree& pt, BadData& error) {
	using namespace boost::property_tree;
	if (pt.empty())
		return;
	if (pt.size() % 2 != 0) {
		error.setDetails("conn size not even");
		throw error;
	}
	pairs.reserve(pt.size() / 2);
	std::pair<int, int> pair;
	ptree::const_iterator it = pt.begin();
	if (!VALID_ARRAY(it, pt.end())) {
		error.setDetails("conn invalid array");
		throw error;
	}
	while (it != pt.end()) {
		//! TODO check valid int
		pair.first = std::stoi(it->second.data());
		++it;
		pair.second = std::stoi(it->second.data());
		++it;
		assert(pair.second > pair.first);
		pairs.push_back(pair);
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
	using namespace boost::property_tree;
	using namespace JSONHelper;
	BadData error{"JSONReader error processing spritesheet"};
	error.setFilePath(filePath);

	std::shared_ptr<SpriteSheetData> data = std::make_shared<SpriteSheetData>();
	ptree pt;
	ptree::const_iterator it;
	std::string tmpStr;
	int tmpInt, i, j;
	read(filePath, pt);
#if defined(DEBUG_JSON_PROC) && DEBUG_JSON_PROC
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "PROC spritesheet " << getRelPathToDataDir(filePath) << std::endl;
#endif

	checkExists(pt, {"img", "sprites"}, error);
	data->image = pt.get<std::string>("img");

	// process sprites
	i = 0;
	SDL_Rect tmpRect;
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("sprites")) {
		if (!v.first.empty()) {
			error.setDetails("sprites: unexpected object at index " + std::to_string(i));
			throw error;
		}
		if (v.second.size() != 5) {
			error.setDetails("sprites: invalid size at index " + std::to_string(i));
			throw error;
		}

		it = v.second.begin();
		tmpStr = it->second.data();

		for (++it, j = 0; it != v.second.end(); ++it, ++j) {
			if (!it->first.empty()) {
				error.setDetails("sprites: unexpected object at index " + std::to_string(i));
				throw error;
			}
			if (!IS_UINT(it->second.data())) {
				error.setDetails("sprites: invalid integer at index " + std::to_string(i));
				throw error;
			}
			tmpInt = std::stoi(it->second.data());
			if (!EQUIVALENT(it->second.data(), tmpInt)) {
				error.setDetails("sprites: invalid integer at index " + std::to_string(i));
				throw error;
			}
			switch (j) {
			case 0:
				tmpRect.x = tmpInt;
				break;
			case 1:
				tmpRect.y = tmpInt;
				break;
			case 2:
				tmpRect.w = tmpInt;
				break;
			case 3:
				tmpRect.h = tmpInt;
				break;
			default:
				break;
			}
		}
#ifndef NDEBUG
		if (data->sprites.find(tmpStr) != data->sprites.end()) {
			error.setDetails("sprite " + tmpStr + " is not unique");
			throw error;
		}
#endif // NDEBUG
		data->sprites.emplace(tmpStr, tmpRect);
		++i;
	}

	return data;
}


std::shared_ptr<RoomData> JSONReader::runLoadRoom(const std::string& filePath) {
	using namespace boost::property_tree;
	using namespace JSONHelper;
	BadData error{"JSONReader error processing room"};
	error.setFilePath(filePath);

	std::shared_ptr<RoomData> data = std::make_shared<RoomData>();
	ptree pt;
	std::string tmpStr;
	RoomSpriteData tmpRS;
	RoomCreatureData tmpCD;
	SDL_Rect tmpRect;
	int tmpInt;

	read(filePath, pt);
#if defined(DEBUG_JSON_PROC) && DEBUG_JSON_PROC
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "PROC room " << getRelPathToDataDir(filePath) << std::endl;
#endif

	checkExists(
		pt,
		{"background", "block", "creatures", "conn", "items"},
		error
	);

	// process background
	int i = 0;
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("background")) {
		if (!v.first.empty()) {
			error.setDetails("\"background\" expecting array");
			throw error;
		}
		if (v.second.count("name") != 1) {
			error.setDetails("\"background\" missing name at index " + std::to_string(i));
			throw error;
		}
		if (v.second.count("x") != 1) {
			error.setDetails("\"background\" missing x at index " + std::to_string(i));
			throw error;
		}
		if (v.second.count("y") != 1) {
			error.setDetails("\"background\" missing y at index " + std::to_string(i));
			throw error;
		}
		tmpRS.name = v.second.get<std::string>("name");
		tmpStr = v.second.get<std::string>("x");
		if (!IS_UINT(tmpStr)) {
			error.setDetails("\"background\" expecting integer x at index " + std::to_string(i));
			throw error;
		}
		tmpRS.x = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpRS.x)) {
			error.setDetails("\"background\" invalid x at index " + std::to_string(i));
			throw error;
		}
		tmpStr = v.second.get<std::string>("y");
		if (!IS_UINT(tmpStr)) {
			error.setDetails("\"background\" expecting integer y at index " + std::to_string(i));
			throw error;
		}
		tmpRS.y = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpRS.y)) {
			error.setDetails("\"background\" invalid y at index " + std::to_string(i));
			throw error;
		}
		data->bg.push_back(tmpRS);

		++i;
	}
	// process block
	i = 0;
	int i2;
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("block")) {
		if (!v.first.empty()) {
			error.setDetails("\"block\" expecting array at index " + std::to_string(i));
			throw error;
		}
		if (v.second.size() != 4) {
			error.setDetails("\"block\" invalid array size at index " + std::to_string(i));
			throw error;
		}
		i2 = 0;
		BOOST_FOREACH(ptree::value_type& v2, v.second) {
			if (!v2.first.empty()) {
				// iterating over integers in array, so first should always be empty
				error.setDetails(
					"\"block\" unexpected object at index " + std::to_string(i) + ", " + std::to_string(i2)
				);
				throw error;
			}

			tmpStr = v2.second.data();
			switch (i2) {
			case 0:		// x
				if (!IS_INT(tmpStr)) {
					error.setDetails(
						"\"block\" expecting integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					error.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpRect.x = tmpInt;
				break;
			case 1:		// y
				if (!IS_INT(tmpStr)) {
					error.setDetails(
						"\"block\" expecting integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					error.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpRect.y = tmpInt;
				break;
			case 2:		// w
				if (!IS_UINT(tmpStr)) {
					error.setDetails(
						"\"block\" expecting unsigned integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					error.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpRect.w = tmpInt;
				break;
			case 3:		// h
				if (!IS_UINT(tmpStr)) {
					error.setDetails(
						"\"block\" expecting unsigned integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					error.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw error;
				}
				tmpRect.h = tmpInt;
				data->block.push_back(tmpRect);
				break;
			default:
				// shouldn't happen...
				break;
			}
			++i2;
		}
		++i;
	}
	// process creatures
	i = 0;
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("creatures")) {
		if (!v.first.empty()) {
			error.setDetails("\"creatures\" expecting array");
			throw error;
		}
		if (v.second.size() != 3) {
			error.setDetails("\"creatures\" invalid object size at index " + std::to_string(i));
			throw error;
		}
		if (v.second.count("name") == 0) {
			error.setDetails("\"creatures\" missing name at index " + std::to_string(i));
			throw error;
		}
		if (v.second.count("x") == 0) {
			error.setDetails("\"creatures\" missing x at index " + std::to_string(i));
			throw error;
		}
		if (v.second.count("y") == 0) {
			error.setDetails("\"creatures\" missing y at index " + std::to_string(i));
			throw error;
		}
		tmpCD.name = v.second.get<std::string>("name");
		tmpStr = v.second.get<std::string>("x");
		if (!IS_UINT(tmpStr)) {
			error.setDetails("\"creatures\" expecting integer x at index " + std::to_string(i));
			throw error;
		}
		tmpInt = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpInt)) {
			error.setDetails("\"creatures\" invalid x at index " + std::to_string(i));
			throw error;
		}
		tmpCD.x = tmpInt;
		tmpStr = v.second.get<std::string>("y");
		if (!IS_UINT(tmpStr)) {
			error.setDetails("\"creatures\" expecting integer y at index " + std::to_string(i));
			throw error;
		}
		tmpInt = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpInt)) {
			error.setDetails("\"creatures\" invalid y at index " + std::to_string(i));
			throw error;
		}
		tmpCD.y = tmpInt;
		data->creatures.push_back(tmpCD);

		++i;
	}
	// process conn
	ptree& ptConn = pt.get_child("conn");
	checkExists(ptConn, {"n", "e", "s", "w"}, error);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::NORTH)],
		ptConn.get_child("n"),
		error
	);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::EAST)],
		ptConn.get_child("e"),
		error
	);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::SOUTH)],
		ptConn.get_child("s"),
		error
	);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::WEST)],
		ptConn.get_child("w"),
		error
	);

	return data;
}


std::shared_ptr<CreatureData> JSONReader::runLoadCreature(const std::string& filePath) {
	using namespace boost::property_tree;
	using namespace JSONHelper;
	BadData error{"JSONReader error processing creature"};
	error.setFilePath(filePath);
	std::shared_ptr<CreatureData> data = std::make_shared<CreatureData>();
	ptree pt;
	std::string tmpStr;
	int i;

	read(filePath, pt);
#if defined(DEBUG_JSON_PROC) && DEBUG_JSON_PROC
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "PROC creature " << getRelPathToDataDir(filePath) << std::endl;
#endif

	checkExists(pt, {"attr"}, error);
	// process attributes
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("attr")) {
		data->attributes[v.first] = v.second.data();
	}
	// process animations
	if (pt.count("animations") != 0) {
		i = 0;
		BOOST_FOREACH(ptree::value_type& v, pt.get_child("animations")) {
			data->animations.emplace_front();
			auto& obj = data->animations.front();
			BOOST_FOREACH(ptree::value_type& v2, v.second) {
				if (v2.first.empty()) {
					error.setDetails("\"animations\" invalid name at index " + std::to_string(i));
					throw error;
				}
				obj[v2.first] = v2.second.data();
			}
			// make sure every animation includes name
			if (obj.count("name") == 0) {
				error.setDetails("\"animations\" object missing name at index " + std::to_string(i));
				throw error;
			}
			++i;
		}
	}
	//! TODO process the rest
	return data;
}
