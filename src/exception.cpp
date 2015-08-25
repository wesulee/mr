#include "exception.h"
#include "logger.h"
#include "sdl_helper.h"
#include <cassert>
#include <cstdlib>
#include <iostream>


static std::string makeMessage(const char* prepend, const std::string& msg, const std::string& det) {
	std::string str = prepend + msg + '.';
	if (!det.empty())
		str += " Details: " + det + '.';
	return str;
}


static void displayMessage(const Uint32 flags, const char* title, const char* message) {
	if (SDL_ShowSimpleMessageBox(flags, title, message, SDL::window) != 0) {
		std::cerr << "Unable to display message box." << std::endl;
	}
}


void logAndExit(const Exception& e) {
	std::cerr << "An error has occurred, see " << Logger::getPath() << " for details.";
	const std::string& str = e.what();
	Logger::instance().log(str);
	displayMessage(SDL_MESSAGEBOX_ERROR, "Error", str.c_str());
	exit(EXIT_FAILURE);
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
	return makeMessage("BadData: ", message, details);
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
	return makeMessage("FileError: ", "file \"" + filePath + "\" " + message, details);
}


ParserError::ParserError(const DataType t) : type(t) {
}


std::string ParserError::what() const {
	std::string prepend;
	switch (type) {
	case DataType::INI:
		prepend += "ini";
		break;
	case DataType::JSON:
		prepend += "json";
		break;
	default:
		assert(false);
	}
	prepend += " ParserError: file \"";
	prepend += filePath;
	prepend += "\" (line " + std::to_string(line) + ") ";
	return makeMessage(prepend.c_str(), message, details);
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


void ParserError::setLine(const int n) {
	line = n;
}
