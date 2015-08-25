#pragma once

#include <boost/property_tree/ptree_fwd.hpp>
#include <memory>
#include <stdexcept>
#include <string>
#include <vector>


class CreatureData;
class SpriteSheetData;
class RoomData;


class JSONReader {
public:
	class ParseError : public std::logic_error {
	public:
		explicit ParseError() : std::logic_error(std::string{}) {}
		void setFunctionName(const std::string& s) {functionName = s;}
		const std::string& getFunctionName() const {return functionName;}
		void setMessage(const std::string& s) {message = s;}
		const std::string& getMessage() const {return message;}
		void setDetails(const std::string& s) {details = s;}
		const std::string& getDetails() const {return details;}
	private:
		std::string functionName;
		std::string message;
		std::string details;
	};

	static std::shared_ptr<SpriteSheetData> loadSpriteSheet(const std::string&);
	static std::shared_ptr<RoomData> loadRoom(const std::string&);
	static std::shared_ptr<CreatureData> loadCreature(const std::string&);
	static std::string getError() {return errorMsg;}
private:
	static std::shared_ptr<SpriteSheetData> runLoadSpriteSheet(const std::string&);
	static std::shared_ptr<RoomData> runLoadRoom(const std::string&);
	static std::shared_ptr<CreatureData> runLoadCreature(const std::string&);
	static std::string getLogString(const ParseError&);
	static void read(const std::string&, boost::property_tree::ptree&, ParseError&);
	static void checkExists(const boost::property_tree::ptree&, const std::vector<std::string>&, ParseError&);

	static std::string errorMsg;
};
