#include "logger.h"
#include "constants.h"
#include "exception.h"
#include "sdl_helper.h"
#include <cstdio>
#include <cstdlib>
#include <ctime>
#include <iostream>


static void displayMessage(const Uint32 flags, const char* title, const char* message) {
	if (SDL_ShowSimpleMessageBox(flags, title, message, SDL::window) != 0) {
		std::cerr << "Unable to display message box." << std::endl;
	}
}


void Logger::setPath(const std::string& s) {
	path = s;
	os = std::ofstream{path, std::ofstream::out | std::ofstream::app};
	if (!os) {
		std::cerr << "Unable to open log file \"" << path << '\"' << std::endl;
		//! ...
	}
}


void Logger::log(const std::string& msg) {
	begin() << msg << std::endl;
	os.flush();
}


void Logger::exit(const Exception& e) {
	std::cerr << "An error has occurred, see \"" << path << "\" for details." << std::endl;
	const std::string& str = e.what();
	log(str);
	displayMessage(SDL_MESSAGEBOX_ERROR, "Error", str.c_str());
	std::exit(EXIT_FAILURE);
}


std::ofstream& Logger::begin() {
	os << getTimestamp() << ' ';
	return os;
}


std::string Logger::getTimestamp() {
	std::time_t now = std::time(0);
	struct tm tstruct;
	char buffer[80];
	tstruct = *std::localtime(&now);
	std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &tstruct);
	return std::string{buffer};
}
