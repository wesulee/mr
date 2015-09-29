#include "json_reader.h"
#include "constants.h"
#include "exception.h"
#include "game_data.h"
#include "json_validator.h"
#include "logger.h"
#include <rapidjson/error/en.h>
#include <rapidjson/filereadstream.h>
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


static void validateRoomConn(JSONValidator& val, const std::string& dir) {
	val.enter(dir);
	val.checkArray();
	val.checkSizeMult(2);
	val.leave();
}


static void doValidateRoom(const rapidjson::Document& data, const std::string& filePath) {
	BadData error{"invalid room data"};
	error.setFilePath(filePath);
	JSONValidator val{&data, error};

	val.checkObject();

	val.enter("background");
	val.checkArray();
	std::size_t arraySz = val.getSize();
	for (std::size_t i = 0; i < arraySz; ++i) {
		val.enter(i);

		val.enter("name");
		val.checkString();
		val.leave();
		val.enter("x");
		val.checkInt();
		val.leave();
		val.enter("y");
		val.checkInt();
		val.leave();

		val.leave();	// i
	}
	val.leave();	// background

	val.enter("block");
	val.checkArray();
	arraySz = val.getSize();
	for (std::size_t i = 0; i < arraySz; ++i) {
		val.enter(i);

		val.checkArray();
		val.checkSize(4);
		for (std::size_t j = 0; j < 4; ++j) {
			val.enter(j);
			val.checkIntGE(0);
			val.leave();
		}

		val.leave();	// i
	}
	val.leave();	// block

	val.enter("creatures");
	val.checkArray();
	arraySz = val.getSize();
	for (std::size_t i = 0; i < arraySz; ++i) {
		val.enter(i);

		val.enter("name");
		val.checkString();
		val.leave();
		val.enter("x");
		val.checkInt();
		val.leave();
		val.enter("y");
		val.checkInt();
		val.leave();

		val.leave();	// i
	}
	val.leave();	// creatures

	val.enter("conn");
	validateRoomConn(val, "n");
	validateRoomConn(val, "e");
	validateRoomConn(val, "s");
	validateRoomConn(val, "w");
	val.leave();	// conn
}


static void doValidateSpriteSheet(const rapidjson::Document& data, const std::string& filePath) {
	BadData error{"invalid spritesheet data"};
	error.setFilePath(filePath);
	JSONValidator val{&data, error};

	val.checkObject();

	val.enter("img");
	val.checkString();
	val.leave();

	val.enter("sprites");
	val.checkArray();
	std::size_t arraySz = val.getSize();
	for (std::size_t i = 0; i < arraySz; ++i) {
		val.enter(i);

		val.checkArray();
		val.checkSize(5);

		val.enter(0);
		val.checkString();
		val.leave();
		for (std::size_t j = 1; j < 5; ++j) {
			val.enter(j);
			val.checkIntGE(0);
			val.leave();
		}

		val.leave();	// i
	}
	val.leave();	// sprites
}

} // namespace JSONHelper


namespace JSONReader {

std::shared_ptr<rapidjson::Document> read(const std::string& filePath) {
	try {
		return read2(filePath);
	}
	catch (Exception const& e) {
		Logger::instance().log(e);
	}
	return nullptr;
}


std::shared_ptr<rapidjson::Document> read2(const std::string& filePath) {
#if defined(DEBUG_JSON_READ) && DEBUG_JSON_READ
	DEBUG_BEGIN << DEBUG_JSON_PREPEND << "READ " << JSONHelper::getRelPathToDataDir(filePath) << std::endl;
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
	std::shared_ptr<rapidjson::Document> doc = std::make_shared<rapidjson::Document>();
	char buffer[Constants::JSONBufferSz];
	rapidjson::FileReadStream is{f, buffer, sizeof(buffer)};
	doc->ParseStream(is);
	std::fclose(f);
	if (doc->HasParseError()) {
		ParserError error{ParserError::DataType::JSON};
		error.setPath(filePath);
		error.setOffset(doc->GetErrorOffset());
		error.setWhat("error parsing json", rapidjson::GetParseError_En(doc->GetParseError()));
		throw error;
	}
	return doc;
}


void validateRoom(const rapidjson::Document& data, const std::string& filePath) {
	try {
		JSONHelper::doValidateRoom(data, filePath);
	}
	catch (Exception const& e) {
		Logger::instance().exit(e);
	}
}


void validateSpriteSheet(const rapidjson::Document& data, const std::string& filePath) {
	try {
		JSONHelper::doValidateSpriteSheet(data, filePath);
	}
	catch (Exception const& e) {
		Logger::instance().exit(e);
	}
}

} // namespace JSONReader
