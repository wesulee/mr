#pragma once

#include <string>


class Logger {
	Logger(Logger const&) = delete;
	void operator=(Logger const&) = delete;
public:
	~Logger() = default;
	static Logger& instance(void);
	static void setPath(const std::string&);
	static const std::string& getPath(void);
	void log(const std::string&);
private:
	Logger() = default;
	static std::string getTimestamp(void);

	static std::string path;
};


inline
Logger& Logger::instance() {
	static Logger instance;
	return instance;
}


inline
void Logger::setPath(const std::string& s) {
	path = s;
}


inline
const std::string& Logger::getPath() {
	return path;
}
