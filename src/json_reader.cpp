#include "json_reader.h"
#include "constants.h"
#include "exception.h"
#include "game_data.h"
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

} // namespace JSONReader
