#pragma once

#include <fstream>
#include <string>


class Exception;


class Logger {
	Logger(Logger const&) = delete;
	void operator=(Logger const&) = delete;
public:
	~Logger() = default;
	static Logger& instance(void);
	void setPath(const std::string&);
	const std::string& getPath(void);
	void log(const std::string&);
	void log(const Exception&);
	void exit(const Exception&);
private:
	Logger() = default;
	std::ofstream& begin(void);
	static std::string getTimestamp(void);

	std::ofstream os;
	std::string path;
};


inline
Logger& Logger::instance() {
	static Logger instance;
	return instance;
}


inline
const std::string& Logger::getPath() {
	return path;
}
