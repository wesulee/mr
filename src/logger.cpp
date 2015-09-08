#include "logger.h"
#include "constants.h"
#include <cstdio>
#include <ctime>
#include <iostream>


void Logger::setPath(const std::string& s) {
	path = s;
	os = std::ofstream{path, std::ofstream::out | std::ofstream::app};
	if (!os) {
		std::cerr << "Unable to open log file \"" << path << '\"' << std::endl;
		//! ...
	}
}


void Logger::log(const std::string& msg) {
	os << getTimestamp() << ' ' << msg << std::endl;
	os.flush();
}


std::string Logger::getTimestamp() {
	time_t now = time(0);
	struct tm tstruct;
	char buffer[80];
	tstruct = *localtime(&now);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &tstruct);
	return std::string{buffer};
}
