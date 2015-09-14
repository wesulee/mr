#pragma once

#include <memory>
#include <string>


class CreatureData;
class SpriteSheetData;
class RoomData;


class JSONReader {
public:
	static std::shared_ptr<SpriteSheetData> loadSpriteSheet(const std::string&);
	static std::shared_ptr<RoomData> loadRoom(const std::string&);
	static std::shared_ptr<CreatureData> loadCreature(const std::string&);
private:
	static std::shared_ptr<SpriteSheetData> runLoadSpriteSheet(const std::string&);
	static std::shared_ptr<RoomData> runLoadRoom(const std::string&);
	static std::shared_ptr<CreatureData> runLoadCreature(const std::string&);
};
