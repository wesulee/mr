#include "ini_parser.h"
#include "exception.h"
#include "logger.h"
#include <boost/filesystem.hpp>
#include <cassert>
#include <cctype>	// isalpha
#include <fstream>
#include <stdexcept>
#define BOOST_FILESYSTEM_NO_DEPRECATED


static void checkSection(const std::string& str, ParserError& error) {
	assert(!str.empty());
	assert(str.front() == '[');
	for (std::size_t i = 1; i < str.size(); ++i) {
		if (!std::isalpha(str[i])) {
			if (str[i] != ']') {
				error.setWhat("section name must be alphabetic");
				throw error;
			}
			if ((i + 1) != str.size()) {
				error.setWhat("section must end with ']'");
				throw error;
			}
		}
	}
}


IniParser::ValueMap IniParser::readFile(const std::string& filePath) {
	try {
		return read(filePath);
	}
	catch (Exception const& e) {
		Logger::instance().exit(e);
	}
	throw std::runtime_error("IniParser::readFile");
}


IniParser::ValueMap IniParser::read(const std::string& filePath) {
	IniRead info;
	ValueMap result;
	info.error.setPath(filePath);
	info.error.setType(ParserError::DataType::INI);
	info.str.resize(1024);	// unlikely that any line would be longer than this...
	info.values = &result;
	if (!boost::filesystem::exists(filePath))
		Logger::instance().exit(FileError{filePath, FileError::Err::MISSING});
	std::ifstream f{filePath};
	if (!f.is_open())
		Logger::instance().exit(FileError{filePath, FileError::Err::NOT_OPEN});
	info.f = &f;
	IniParser::doRead(info);
	return result;
}


void IniParser::doRead(IniRead& info) {
	while (*info.f) {
		info.error.incLine();
		readLine(info);
	}
	if (!info.f->eof()) {
		info.error.setWhat("i/o error");
		throw info.error;
	}
}


void IniParser::readLine(IniRead& info) {
	if (!std::getline(*info.f, info.str)) {
		if (!info.f->eof()) {
			info.error.setWhat("i/o error");
			throw info.error;
		}
	}

	if (info.str.empty())
		;	// empty line
	else if (info.str[0] == IniSettings::COMMENT) {
		info.str.clear();
	}
	else if (info.str[0] == '[') {
		checkSection(info.str, info.error);	// check valid section
		info.str.clear();
	}
	else {
		readPair(info);
		info.str.clear();
	}
}


void IniParser::readPair(IniRead& info) {
	// read name
	std::size_t setIndex = std::string::npos;
	for (std::size_t i = 0; i < info.str.size(); ++i) {
		if (info.str[i] == IniSettings::SET) {
			setIndex = i;
			break;
		}
		if (!std::isalpha(info.str[i])) {
			info.error.setWhat("name must be alphabetic");
			throw info.error;
		}
	}
	if (setIndex == std::string::npos) {
		info.error.setWhat("name unexpected end");
		throw info.error;
	}
	info.tmp.assign(info.str, 0, setIndex);
	auto insert = info.values->emplace(info.tmp, std::string{});
	if (!insert.second && !IniSettings::DUPLICATE_NAME) {
		// duplicate name
		info.error.setWhat("duplicate name " + info.tmp);
		throw info.error;
	}
	// read value
	if ((setIndex + 1) == info.str.size())
		return;	// empty value
	insert.first->second.assign(info.str, setIndex + 1, std::string::npos);
}
