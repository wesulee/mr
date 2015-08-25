#include "logger.h"
#include <cstdio>
#include <ctime>
#include <fstream>


std::string Logger::path = "errors.txt";


void Logger::log(const std::string& msg) {
	std::ofstream f{path, std::ofstream::out | std::ofstream::app};
	if (!f) {
		// what to do?
		return;
	}

	f << getTimestamp() << ' ' << msg << std::endl;
}


std::string Logger::getTimestamp() {
	time_t now = time(0);
	struct tm tstruct;
	char buffer[80];
	tstruct = *localtime(&now);
	strftime(buffer, sizeof(buffer), "%Y-%m-%d %X", &tstruct);
	return std::string{buffer};
}
