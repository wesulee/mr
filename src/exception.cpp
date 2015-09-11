#include "exception.h"
#include "sdl_header.h"
#include "utility.h"	// q
#include <cassert>
#include <limits>


namespace ExceptionHelper {
	constexpr std::size_t defaultLine = std::numeric_limits<std::size_t>::max();
	constexpr std::size_t defaultOffset = defaultLine;
}


static std::string makeMessage(const char* prepend, const std::string& msg, const std::string& det) {
	std::string str = prepend + msg + '.';
	if (!det.empty()) {
		str += " Details: " + det;
		if (str.back() != '.')
			str += '.';
	}
	return str;
}


RuntimeError::RuntimeError(const std::string& msg, const std::string& det) : message(msg), details(det) {
}


std::string RuntimeError::what() const {
	return makeMessage("RuntimeError: ", message, details);
}


SDLError::SDLError(const std::string& msg, const char* func) : message(msg), funcName(func) {
	sdlMessage = SDL_GetError();
}


std::string SDLError::what() const {
	std::string str = "SDLError: " + message + ". ";
	str += funcName;
	str += "(): ";
	str += sdlMessage;
	return str;
}


BadData::BadData(const std::string& msg, const std::string& det) : message(msg), details(det) {
}


std::string BadData::what() const {
	if (filePath.empty())
		return makeMessage("BadData: ", message, details);
	else
		return makeMessage("BadData: ", "file " + q(filePath) + ' ' + message, details);
}


FileError::FileError(const std::string& file, const std::string& msg, const std::string& det)
: filePath(file), message(msg), details(det) {
}


FileError::FileError(const std::string& file, const Err etype, const std::string& det)
: filePath(file), details(det) {
	switch (etype) {
	case Err::MISSING:
		message = "does not exist";
		break;
	case Err::NOT_OPEN:
		message = "unable to open";
		break;
	case Err::NOT_FILE:
		message = "is not a file";
		break;
	case Err::NOT_DIRECTORY:
		message = "is not a directory";
		break;
	}
}


std::string FileError::what() const {
	return makeMessage("FileError: ", "file " + q(filePath) + ' ' + message, details);
}


ParserError::ParserError(const DataType t) : line(ExceptionHelper::defaultLine),
offset(ExceptionHelper::defaultOffset), type(t) {
}


std::string ParserError::what() const {
	std::string str{"type "};
	switch (type) {
	case DataType::INI:
		str += "INI";
		break;
	case DataType::JSON:
		str += "JSON";
		break;
	default:
		assert(false);
		str += "???";
	}
	str += " file ";
	str += q(filePath);
	str += ' ';
	if (line != ExceptionHelper::defaultLine) {
		str += "(line " + std::to_string(line) + ") ";
	}
	else if (offset != ExceptionHelper::defaultOffset) {
		str += "(offset " + std::to_string(offset) + ") ";
	}
	str += message;
	return makeMessage("ParserError: ", str, details);
}


void ParserError::setPath(const std::string& path) {
	filePath = path;
}


void ParserError::setWhat(const std::string& msg, const std::string& det) {
	message = msg;
	details = det;
}


void ParserError::setType(const DataType t) {
	type = t;
}


void ParserError::setLine(const std::size_t n) {
	line = n;
}


void ParserError::setOffset(const std::size_t n) {
	offset = n;
}
