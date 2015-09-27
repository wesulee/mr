#pragma once

#include <boost/variant/variant.hpp>
#include <rapidjson/document.h>
#include <limits>
#include <stack>
#include <string>


class BadData;


// Throws BadData exception when fails validation
// Throws RuntimeError if leave() called too many times
class JSONValidator {
	JSONValidator(const JSONValidator&) = delete;
	void operator=(const JSONValidator&) = delete;

	enum class DataType {NONE, NULL_, BOOLEAN, OBJECT, ARRAY, STRING, NUMBER};	// JSON data types
public:
	struct Path {
		// the key used on previous container that brought it to current value
		boost::variant<std::size_t, std::string> key = std::numeric_limits<std::size_t>::max();
		const rapidjson::Value* value = nullptr;
		DataType valueType = DataType::NONE;
	};

	JSONValidator(const rapidjson::Document*, BadData&);
	~JSONValidator() {}
	void enter(const std::size_t);
	void enter(const std::string&);
	void leave(void);
	void checkArray(void);
	void checkInt(void);
	void checkIntGE(const int);
	void checkObject(void);
	void checkString(void);
	void checkUint(void);
	void checkKey(const std::size_t);
	void checkKey(const std::string&);
	void checkSize(const std::size_t);
	void checkSizeMult(const std::size_t);
	bool exists(const std::string&);
	int getInt(void);
	unsigned int getUint(void);
	std::size_t getSize(void);
	std::string getString(void);
private:
	void throwError(const std::string&);

	std::stack<Path> stack;
	BadData& error;
};
