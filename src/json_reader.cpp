#include "json_reader.h"
#include "data_validation.h"
#include "json_data.h"
#include "logger.h"
#include "room_inc.h"
#include "sdl_header.h"
#include <boost/foreach.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <cassert>


std::string JSONReader::errorMsg = "";


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


static void procRoomConnecting(std::vector<std::pair<int, int>>& pairs,
	const boost::property_tree::ptree& pt, JSONReader::ParseError& pErr) {
	using namespace boost::property_tree;
	if (pt.empty())
		return;
	if (pt.size() % 2 != 0) {
		pErr.setDetails("conn size not even");
		throw pErr;
	}
	pairs.reserve(pt.size() / 2);
	std::pair<int, int> pair;
	ptree::const_iterator it = pt.begin();
	if (!VALID_ARRAY(it, pt.end())) {
		pErr.setDetails("conn invalid array");
		throw pErr;
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


std::shared_ptr<SpriteSheetData> JSONReader::loadSpriteSheet(const std::string& filePath) {
	std::shared_ptr<SpriteSheetData> data;
	try {
		data = runLoadSpriteSheet(filePath);
	}
	catch (ParseError const& e) {
		Logger::instance().log(getLogString(e));
		errorMsg = e.getMessage();
	}
	return data;
}


std::shared_ptr<RoomData> JSONReader::loadRoom(const std::string& filePath) {
	std::shared_ptr<RoomData> data;
	try {
		data = runLoadRoom(filePath);
	}
	catch (ParseError const& e) {
		Logger::instance().log(getLogString(e));
		errorMsg = e.getMessage();
	}
	return data;
}


std::shared_ptr<CreatureData> JSONReader::loadCreature(const std::string& filePath) {
	std::shared_ptr<CreatureData> data;
	try {
		data = runLoadCreature(filePath);
	}
	catch (ParseError const& e) {
		Logger::instance().log(getLogString(e));
		errorMsg = e.getMessage();
	}
	return data;
}


std::shared_ptr<SpriteSheetData> JSONReader::runLoadSpriteSheet(const std::string& filePath) {
	using namespace boost::property_tree;
	ParseError pErr;
	pErr.setFunctionName("JSONReader::loadSpriteSheet");
	pErr.setMessage("error processing spritesheet");

	std::shared_ptr<SpriteSheetData> data = std::make_shared<SpriteSheetData>();
	ptree pt;
	ptree::const_iterator it;
	std::string tmpStr;
	int tmpInt, i, j;
	read(filePath, pt, pErr);

	checkExists(pt, {"img", "sprites"}, pErr);
	data->image = pt.get<std::string>("img");

	// process sprites
	i = 0;
	SDL_Rect tmpRect;
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("sprites")) {
		if (!v.first.empty()) {
			pErr.setDetails("sprites: unexpected object at index " + std::to_string(i));
			throw pErr;
		}
		if (v.second.size() != 5) {
			pErr.setDetails("sprites: invalid size at index " + std::to_string(i));
			throw pErr;
		}

		it = v.second.begin();
		tmpStr = it->second.data();

		for (++it, j = 0; it != v.second.end(); ++it, ++j) {
			if (!it->first.empty()) {
				pErr.setDetails("sprites: unexpected object at index " + std::to_string(i));
				throw pErr;
			}
			if (!IS_UINT(it->second.data())) {
				pErr.setDetails("sprites: invalid integer at index " + std::to_string(i));
				throw pErr;
			}
			tmpInt = std::stoi(it->second.data());
			if (!EQUIVALENT(it->second.data(), tmpInt)) {
				pErr.setDetails("sprites: invalid integer at index " + std::to_string(i));
				throw pErr;
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
			pErr.setDetails("sprite " + tmpStr + " is not unique");
			throw pErr;
		}
#endif // NDEBUG
		data->sprites.emplace(tmpStr, tmpRect);
		++i;
	}

	return data;
}


std::shared_ptr<RoomData> JSONReader::runLoadRoom(const std::string& filePath) {
	using namespace boost::property_tree;
	ParseError pErr;
	pErr.setFunctionName("JSONReader::loadRoom");
	pErr.setMessage("error processing room");

	std::shared_ptr<RoomData> data = std::make_shared<RoomData>();
	ptree pt;
	std::string tmpStr;
	RoomSpriteData tmpRS;
	RoomCreatureData tmpCD;
	SDL_Rect tmpRect;
	int tmpInt;

	read(filePath, pt, pErr);

	checkExists(
		pt,
		{"background", "block", "creatures", "conn", "items"},
		pErr
	);

	// process background
	int i = 0;
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("background")) {
		if (!v.first.empty()) {
			pErr.setDetails("\"background\" expecting array");
			throw pErr;
		}
		if (v.second.count("name") != 1) {
			pErr.setDetails("\"background\" missing name at index " + std::to_string(i));
			throw pErr;
		}
		if (v.second.count("x") != 1) {
			pErr.setDetails("\"background\" missing x at index " + std::to_string(i));
			throw pErr;
		}
		if (v.second.count("y") != 1) {
			pErr.setDetails("\"background\" missing y at index " + std::to_string(i));
			throw pErr;
		}
		tmpRS.name = v.second.get<std::string>("name");
		tmpStr = v.second.get<std::string>("x");
		if (!IS_UINT(tmpStr)) {
			pErr.setDetails("\"background\" expecting integer x at index " + std::to_string(i));
			throw pErr;
		}
		tmpRS.x = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpRS.x)) {
			pErr.setDetails("\"background\" invalid x at index " + std::to_string(i));
			throw pErr;
		}
		tmpStr = v.second.get<std::string>("y");
		if (!IS_UINT(tmpStr)) {
			pErr.setDetails("\"background\" expecting integer y at index " + std::to_string(i));
			throw pErr;
		}
		tmpRS.y = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpRS.y)) {
			pErr.setDetails("\"background\" invalid y at index " + std::to_string(i));
			throw pErr;
		}
		data->bg.push_back(tmpRS);

		++i;
	}
	// process block
	i = 0;
	int i2;
	BOOST_FOREACH(ptree::value_type& v, pt.get_child("block")) {
		if (!v.first.empty()) {
			pErr.setDetails("\"block\" expecting array at index " + std::to_string(i));
			throw pErr;
		}
		if (v.second.size() != 4) {
			pErr.setDetails("\"block\" invalid array size at index " + std::to_string(i));
			throw pErr;
		}
		i2 = 0;
		BOOST_FOREACH(ptree::value_type& v2, v.second) {
			if (!v2.first.empty()) {
				// iterating over integers in array, so first should always be empty
				pErr.setDetails(
					"\"block\" unexpected object at index " + std::to_string(i) + ", " + std::to_string(i2)
				);
				throw pErr;
			}

			tmpStr = v2.second.data();
			switch (i2) {
			case 0:		// x
				if (!IS_INT(tmpStr)) {
					pErr.setDetails(
						"\"block\" expecting integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					pErr.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
				}
				tmpRect.x = tmpInt;
				break;
			case 1:		// y
				if (!IS_INT(tmpStr)) {
					pErr.setDetails(
						"\"block\" expecting integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					pErr.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
				}
				tmpRect.y = tmpInt;
				break;
			case 2:		// w
				if (!IS_UINT(tmpStr)) {
					pErr.setDetails(
						"\"block\" expecting unsigned integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					pErr.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
				}
				tmpRect.w = tmpInt;
				break;
			case 3:		// h
				if (!IS_UINT(tmpStr)) {
					pErr.setDetails(
						"\"block\" expecting unsigned integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
				}
				tmpInt = std::stoi(tmpStr);
				if (!EQUIVALENT(tmpStr, tmpInt)) {
					pErr.setDetails(
						"\"block\" invalid integer at index " + std::to_string(i) + ", " + std::to_string(i2)
					);
					throw pErr;
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
			pErr.setDetails("\"creatures\" expecting array");
			throw pErr;
		}
		if (v.second.size() != 3) {
			pErr.setDetails("\"creatures\" invalid object size at index " + std::to_string(i));
			throw pErr;
		}
		if (v.second.count("name") == 0) {
			pErr.setDetails("\"creatures\" missing name at index " + std::to_string(i));
			throw pErr;
		}
		if (v.second.count("x") == 0) {
			pErr.setDetails("\"creatures\" missing x at index " + std::to_string(i));
			throw pErr;
		}
		if (v.second.count("y") == 0) {
			pErr.setDetails("\"creatures\" missing y at index " + std::to_string(i));
			throw pErr;
		}
		tmpCD.name = v.second.get<std::string>("name");
		tmpStr = v.second.get<std::string>("x");
		if (!IS_UINT(tmpStr)) {
			pErr.setDetails("\"creatures\" expecting integer x at index " + std::to_string(i));
			throw pErr;
		}
		tmpInt = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpInt)) {
			pErr.setDetails("\"creatures\" invalid x at index " + std::to_string(i));
			throw pErr;
		}
		tmpCD.x = tmpInt;
		tmpStr = v.second.get<std::string>("y");
		if (!IS_UINT(tmpStr)) {
			pErr.setDetails("\"creatures\" expecting integer y at index " + std::to_string(i));
			throw pErr;
		}
		tmpInt = std::stoi(tmpStr);
		if (!EQUIVALENT(tmpStr, tmpInt)) {
			pErr.setDetails("\"creatures\" invalid y at index " + std::to_string(i));
			throw pErr;
		}
		tmpCD.y = tmpInt;
		data->creatures.push_back(tmpCD);

		++i;
	}
	// process conn
	ptree& ptConn = pt.get_child("conn");
	checkExists(ptConn, {"n", "e", "s", "w"}, pErr);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::NORTH)],
		ptConn.get_child("n"),
		pErr
	);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::EAST)],
		ptConn.get_child("e"),
		pErr
	);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::SOUTH)],
		ptConn.get_child("s"),
		pErr
	);
	procRoomConnecting(
		data->connecting[SideToIndex(Side::WEST)],
		ptConn.get_child("w"),
		pErr
	);

	return data;
}


std::shared_ptr<CreatureData> JSONReader::runLoadCreature(const std::string& filePath) {
	using namespace boost::property_tree;
	ParseError pErr;
	pErr.setFunctionName("JSONReader::loadCreature");
	pErr.setMessage("error processing creature");

	std::shared_ptr<CreatureData> data = std::make_shared<CreatureData>();
	ptree pt;
	std::string tmpStr;
	int i;

	read(filePath, pt, pErr);

	checkExists(pt, {"attr"}, pErr);
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
					pErr.setDetails("\"animations\" invalid name at index " + std::to_string(i));
					throw pErr;
				}
				obj[v2.first] = v2.second.data();
			}
			// make sure every animation includes name
			if (obj.count("name") == 0) {
				pErr.setDetails("\"animations\" object missing name at index " + std::to_string(i));
				throw pErr;
			}
			++i;
		}
	}
	//! TODO process the rest
	return data;
}


std::string JSONReader::getLogString(const ParseError& e) {
	return (e.getFunctionName() + ": " + e.getMessage() + ". Details: " + e.getDetails() + '.');
}


void JSONReader::read(const std::string& filePath, boost::property_tree::ptree& pt, ParseError& pe) {
	using namespace boost::property_tree;
	try {
		read_json(filePath, pt);
	}
	catch (ptree_bad_path const& e) {
		pe.setDetails("bad file path \"" + filePath + '"');
		throw pe;
	}
	catch (json_parser::json_parser_error const& e) {
		pe.setDetails(
			"json_parser_error file \"" + e.filename()
			+ "\" (line " + std::to_string(e.line()) + ") "
			+ e.message()
		);
		throw pe;
	}
}


void JSONReader::checkExists(const boost::property_tree::ptree& pt, const std::vector<std::string>& check, ParseError& pe) {
	for (const auto& str : check) {
		if (pt.count(str) == 0) {
			pe.setDetails("missing \"" + str + '\"');
			throw pe;
		}
	}
}
