#pragma once

#include "exception.h"
#include <istream>
#include <string>
#include <unordered_map>

/*
A very minimal ini parser

Format:
[section]
name=value

Specification:
Case-sensitive
No leading whitespace allowed (trailing ok, but considered part of value)
A section must be on its own line
A section begins with [, ends with ], and section name is alphabetic
A name is alphabetic and delimited from value by SET
A value is any string that follows name delimiter and ends when the line ends
Line endings are handled by C++ runtime (std::getline)
Comments must be on separate line and must begin with COMMENT
Blank lines are ok
*/

namespace IniSettings {
	constexpr char COMMENT = ';';
	constexpr char SET = '=';	// name/value delimiter
	// when true duplicate name overwrite value, when false error is raised
	constexpr bool DUPLICATE_NAME = false;
}


class IniParser {
public:
	typedef std::unordered_map<std::string, std::string> ValueMap;
private:
	enum class DataType {END, NAME, EQUALS, VALUE, COMMENT, IGNORE};
	struct IniRead {
		ParserError error;
		std::string str;
		std::string tmp;
		std::istream* f;
		ValueMap* values;
		char c;
	};
public:
	static ValueMap readFile(const std::string&);
	static ValueMap read(const std::string&);
private:
	static void doRead(IniRead&);
	static void readLine(IniRead&);
	static void readPair(IniRead&);
};
