#include "logger.h"
#include "console.h"
#include "constants.h"
#include "exception.h"
#include "sdl_helper.h"
#include "utility.h"	// q
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
	os.open(path, std::ofstream::out | std::ofstream::app);
	if (!os.is_open()) {
		std::cerr << "Unable to open log file " << q(path) << std::endl;
		//! ...
	}
}


void Logger::log(const std::string& msg) {
	begin() << msg << std::endl;
	os.flush();
}


void Logger::log(const Exception& e) {
	std::string what = e.what();
	Console::begin() << what << std::endl;
	log(what);
}


void Logger::exit(const Exception& e) {
	std::string what = e.what();
	Console::begin() << what << std::endl;
	std::cerr << std::endl << "An error has occurred, see " << q(path) << " for details." << std::endl;
	log(what);
	displayMessage(SDL_MESSAGEBOX_ERROR, "Error", what.c_str());
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
