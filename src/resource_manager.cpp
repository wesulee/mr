#include "resource_manager.h"
#include "animated_sprite.h"
#include "color.h"
#include "constants.h"
#include "data_validation.h"
#include "exception.h"
#include "font.h"
#include "font_resource.h"
#include "game_data.h"
#include "image.h"
#include "json_data.h"
#include "json_reader.h"
#include "logger.h"
#include "save_data.h"
#include "sprite.h"
#include "sprite_sheet.h"
#include "utility.h"
#include <boost/archive/text_iarchive.hpp>
#include <boost/archive/text_oarchive.hpp>
#include <boost/filesystem.hpp>
#include <boost/serialization/vector.hpp>	// SaveData
#include <cassert>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <stdexcept>
#include <vector>
#define BOOST_FILESYSTEM_NO_DEPRECATED


// returns missing key, empty string if ok
template<class T>
static std::string checkMapKeysExists(const T& map, const std::vector<std::string>& keys) {
	for (const auto& k : keys) {
		if (map.count(k) == 0)
			return k;
	}
	return std::string{};
}


inline
static void appendDir(std::string& str, const char* dir) {
	str += dir;
	str += boost::filesystem::path::preferred_separator;
}


static std::string toString(const FontResource& fr) {
	return (
		"name=" + fr.name
		+ " size=" + std::to_string(fr.size)
		+ " shared=" + std::to_string(static_cast<int>(fr.shared))
	);
}


ResourceManager::~ResourceManager() {
	defaultTR.freeFont();
	// don't delete defaultSS
	SDL::freeNull(defaultSurf);
	for (auto it = fonts.begin(); it != fonts.end(); ++it)
		TTF_CloseFont(it->second.reference);
	for (auto f : fontsPrivate)
		TTF_CloseFont(f);
	for (auto it = animations.begin(); it != animations.end(); ++it)
		delete it->second;
	for (auto it = sheets.begin(); it != sheets.end(); ++it)
		delete it->second.reference;
	for (auto it = textures.begin(); it != textures.end(); ++it)
		SDL::free(it->second.reference);
}


void ResourceManager::init() {
	std::pair<SDL_Surface*, SDL_Texture*> img = loadImage("default");
	defaultSurf = img.first;
	defaultSS = loadSpriteSheet("default");
	// populate animation type mapping
	animationLookup.emplace("uni", AnimationType::UNIFORM);
}


SDL_Surface* ResourceManager::getDefaultSurface() {
	return defaultSurf;
}


SpriteSheet* ResourceManager::getSpriteSheet(const std::string& name) {
	auto it = sheets.find(name);
	if (it != sheets.end())
		return it->second.reference;
	// not found, try to load
	return loadSpriteSheet(name);
}


TextRenderer* ResourceManager::getDefaultTR() {
	return &defaultTR;
}


std::shared_ptr<RoomData> ResourceManager::getRoomData(const int x, const int y) {
	std::string filePath = getPath(ResourceType::ROOM, roomToString(x, y));
	std::shared_ptr<RoomData> rd = JSONReader::loadRoom(filePath);
	if (!rd)
		throw std::runtime_error(JSONReader::getError());
	return rd;
}


std::shared_ptr<CreatureData> ResourceManager::getCreatureData(const std::string& name) {
	std::string filePath = getPath(ResourceType::CREATURE, name);
	std::shared_ptr<CreatureData> data = JSONReader::loadCreature(filePath);
	if (!data)
		throw std::runtime_error(JSONReader::getError());
	return data;
}


std::shared_ptr<SaveData> ResourceManager::getSaveData(const std::string& fname) {
	std::string filePath = getPath(ResourceType::SAVE, fname);
	std::shared_ptr<SaveData> data = std::make_shared<SaveData>();
	std::ifstream file{filePath};
	boost::archive::text_iarchive ar{file};
	ar & (*data);
	return data;
}


SDL_Surface* ResourceManager::generateSurface(const RoomData& rd) const {
	SDL_Rect dst;
	SDL_Rect spriteBounds;
	SDL_Surface* surf = SDL::newSurface24(Constants::roomWidth, Constants::roomHeight);
	SDL_FillRect(surf, nullptr, SDL::mapRGB(surf->format, COLOR_BLACK));
	for (auto it = rd.bg.cbegin(); it != rd.bg.cend(); ++it) {
		spriteBounds = defaultSS->getBounds(it->name);
		dst.x = it->x;
		dst.y = it->y;
		dst.w = spriteBounds.w;
		dst.h = spriteBounds.h;
		if (SDL_BlitSurface(defaultSurf, &spriteBounds, surf, &dst) != 0) {
			// error drawing sprite, log error and continue
			SDL::logError("ResourceManager::generateSurface SDL_BlitSurface");
		}
	}
	return surf;
}


FontResource ResourceManager::loadFont(const Font& font, const bool shared) {
	assert(font.size > 0);
	FontResource fr;
	fr.name = font.name;
	fr.size = font.size;
	fr.shared = shared;
	if (shared) {
		// check if already loaded
		auto it = fonts.find(font);
		if (it != fonts.end())	{	// already loaded
			++(it->second.counter);
			fr.font = it->second.reference;
		}
		else {	// hasn't been loaded yet, load now
			ResourceCounter<TTF_Font> insert;
			insert.counter = 1;
			insert.reference = openFont(font);
			// insert into fonts
			fonts[font] = insert;
			fr.font = insert.reference;
		}
	}
	else {	// if not shared, immediately load
		TTF_Font* f = openFont(font);
		fr.font = f;
		auto insert = fontsPrivate.insert(f);
		(void)insert;	// remove warning
		assert(insert.second);
	}
	return fr;
}


// Note: it is an error if unable to find non-null font because there
// must be a logic error for whatever is responsible for loading/unloading it
void ResourceManager::unloadFont(FontResource& fr) {
	if (fr.font == nullptr)
		return;
	if (fr.shared) {
		Font font;
		font.name = fr.name;
		font.size = fr.size;
		auto it = fonts.find(font);
		if (it == fonts.end()) {
			logAndExit(RuntimeError{
				"FontResource error",
				"ResourceManager::unloadFont invalid resource: " + toString(fr)
			});
		}
		--(it->second.counter);
		if (it->second.counter == 0) {	// unload
			TTF_CloseFont(it->second.reference);
			fonts.erase(it);
		}
	}
	else {
		auto it = fontsPrivate.find(fr.font);
		if (it == fontsPrivate.end()) {
			logAndExit(RuntimeError{
				"FontResource error",
				"ResourceManager::unloadFont invalid resource: " + toString(fr)
			});
		}
		fontsPrivate.erase(it);
	}
}


//! TODO rename
Sprite ResourceManager::getSprite(const std::string& name) {
	return defaultSS->get(name);
}


UniformAnimatedSpriteSource* ResourceManager::getUSprSrc(const std::string& name) {
	auto it = animations.find(name);
	if (it == animations.end()) {
		logAndExit(RuntimeError{
			"invalid animation reference",
			"ResourceManager::getUSprSrc invalid reference to " + name
		});
	}
	return dynamic_cast<UniformAnimatedSpriteSource*>(it->second);
}


AnimatedSpriteSource* ResourceManager::loadAnimation(const std::map<std::string, std::string>& args) {
	assert(args.count("name") != 0);
	assert(args.count("img") != 0);
	assert(args.count("type") != 0);
	auto it = animationLookup.find(args.at("type"));
	if (it == animationLookup.end()) {
		logAndExit(RuntimeError{
			"invalid animation type",
			"ResourceManager::loadAnimation unknown type: " + args.at("type")
		});
	}
	const AnimationType t = it->second;
	const std::string& name = args.at("name");
	AnimatedSpriteSource* src;
	switch (t) {
	case AnimationType::UNIFORM:
		src = loadAnimationUni(args);
		break;
	default:
		assert(false);
		src = nullptr;
	}
	if (src == nullptr) {
		logAndExit(RuntimeError{
			"unable to load animation",
			"ResourceManager::loadAnimation name: " + name
		});
	}
	animations[name] = src;
	return src;
}


void ResourceManager::saveSaveData(const std::string& fname, const SaveData& data) {
	std::string filePath = getPath(ResourceType::SAVE, fname);
	std::ofstream file{filePath};
	boost::archive::text_oarchive ar{file};
	ar & data;
}


void ResourceManager::freeAnimation(const std::string& name) {
	auto it = animations.find(name);
	if (it == animations.end()) {
		logAndExit(RuntimeError{
			"invalid animation reference",
			"ResourceManager::freeAnimation cannot find: " + name
		});
	}
	decTextureRef(it->second->getImageName());
	delete it->second;
	animations.erase(it);
}


void ResourceManager::freeSpriteSheet(const std::string& name) {
	auto it = sheets.find(name);
	if (it == sheets.end()) {
		logAndExit(RuntimeError{
			"invalid spritesheet reference",
			"ResourceManager::freeSpriteSheet cannot find: " + name
		});
	}
	--(it->second.counter);
	if (it->second.counter == 0) {
		decTextureRef(it->second.reference->getImageName());
		delete it->second.reference;
		sheets.erase(it);
	}
}


// caller must free returned surface, but not texture
// throws exception on error
std::pair<SDL_Surface*, SDL_Texture*> ResourceManager::loadImage(const std::string& name) {
	assert(textures.find(name) == textures.end());
	std::pair<SDL_Surface*, SDL_Texture*> ret;
	std::string path = getPath(ResourceType::IMAGE, name);
	ret.first = SDL::loadBMP(path);
	std::pair<bool, Color> colorKey = readColorKey(name);
	if (colorKey.first && !SDL::setColorKey(ret.first, colorKey.second)) {
		SDL::logError("ResourceManager::loadImage SDL::setColorKey");
	}
	ret.second = SDL::newTexture(ret.first);
	// keep track of references
	textures[name] = {ret.second, 1};
	return ret;
}


// load image if it hasn't been loaded, else return existing texture
// Note: increments texture reference
SDL_Texture* ResourceManager::autoLoadImage(const std::string& name) {
	auto it = textures.find(name);
	if (it != textures.end()) {
		incTextureRef(name);
		return it->second.reference;
	}
	std::pair<SDL_Surface*, SDL_Texture*> image = loadImage(name);
	SDL::free(image.first);
	return image.second;
}


// throws exception on error
SpriteSheet* ResourceManager::loadSpriteSheet(const std::string& name) {
	assert(sheets.find(name) == sheets.end());
	std::string filePath = getPath(ResourceType::SPRITE, name);
	std::shared_ptr<SpriteSheetData> data = JSONReader::loadSpriteSheet(filePath);
	if (!data)
		throw std::runtime_error(JSONReader::getError());
	// load image
	SDL_Texture* tex = autoLoadImage(data->image);
	SpriteSheet* ss = new SpriteSheet{tex, name};
	sheets[name] = ResourceCounter<SpriteSheet>{ss, 1};
	// process sprites
#ifndef NDEBUG
	// for checking bounds
	int width = -1;
	int height = -1;
	SDL_QueryTexture(tex, nullptr, nullptr, &width, &height);
#endif // NDEBUG
	for (auto it = data->sprites.begin(); it != data->sprites.end(); ++it) {
#ifndef NDEBUG
		if ((it->second.x + it->second.w > width) || (it->second.y + it->second.h > height)) {
			// log if sprite boundary not within bounds, but keep processing
			Logger::instance().log("ResourceManager::loadSpriteSheet " + name + ',' + it->first + " exceeds bounds");
		}
#endif // NDEBUG
		ss->add(it->first, it->second);
	}
	return ss;
}


AnimatedSpriteSource* ResourceManager::loadAnimationUni(const std::map<std::string, std::string>& args) {
	UniformAnimatedSpriteSource* src = new UniformAnimatedSpriteSource;
	src->setImageName(args.at("img"));
#ifndef NDEBUG
	std::string missing = checkMapKeysExists(args, {"img", "dur", "w", "h", "x", "y", "dx", "dy", "frames"});
	if (!missing.empty()) {
		Logger::instance().log("ResourceManager::loadAnimationUni missing argument " + missing);
		delete src;
		return nullptr;
	}
#endif // NDEBUG
	int tmpInt, tmpInt2;
	int x, y, dx, dy, frames;
	src->setTexture(autoLoadImage(args.at("img")));
	tmpInt = std::stoi(args.at("dur"));
	src->setTicks(static_cast<unsigned int>(tmpInt));
	tmpInt = std::stoi(args.at("w"));
	tmpInt2 = std::stoi(args.at("h"));
	src->setSize(tmpInt, tmpInt2);
	x = std::stoi(args.at("x"));
	y = std::stoi(args.at("y"));
	dx = std::stoi(args.at("dx"));
	dy = std::stoi(args.at("dy"));
	frames = std::stoi(args.at("frames"));
	for (int i = 0; i < frames; ++i, x += dx, y += dy)
		src->add(x, y);
	return src;
}


void ResourceManager::incTextureRef(const std::string& name) {
	auto it = textures.find(name);
	assert(it != textures.end());
	++(it->second.counter);
}


void ResourceManager::decTextureRef(const std::string& name) {
	auto it = textures.find(name);
	assert(it != textures.end());
	--(it->second.counter);
	if (it->second.counter == 0) {	// free texture
		SDL::free(it->second.reference);
		textures.erase(it);
	}
}


TTF_Font* ResourceManager::openFont(const Font& font) {
	std::string filePath = getPath(ResourceType::FONT, font.name);
	TTF_Font* f = TTF_OpenFont(filePath.c_str(), font.size);
	if (f == nullptr) {
		SDL::logError("ResourceManager::openFont TTF_OpenFont");
		throw std::invalid_argument("unable to open font");
	}
	return f;
}


// NOTE: invalid color key will return false rather than throw exception
// the only valid color key format is "r,g,b"
std::pair<bool, Color> ResourceManager::readColorKey(const std::string& name) {
	std::pair<bool, Color> ret;
	ret.first = false;
	std::string inputStr;
	std::string filePath = getPath(ResourceType::IMAGE_KEY, name);
	std::ifstream f{filePath};
	if (!f.is_open())
		return ret;
	std::getline(f, inputStr);
	if ((inputStr.size() > 11) || inputStr.empty())	// max valid file size = 3*3 + 2
		return ret;
	inputStr += ',';
	// process file
	unsigned int input = 0;
	int count = 0;
	char c;
	for (std::size_t i = 0; i < inputStr.size(); ++i) {
		c = inputStr[i];
		if (c == ',') {
			readColorKeyProc(ret.second, input, count);
			input = 0;
			if (++count == 3)
				break;
		}
		else {
			if (!((c >= '0') && (c <= '9')))
				return ret;
			input = (input * 10) + static_cast<decltype(input)>(c - '0');
			if (input > 255)
				return ret;
		}
	}
	ret.first = (count == 3);
	return ret;
}


void ResourceManager::readColorKeyProc(Color& c, const unsigned int input, const int count) {
	assert(input <= 255);
	decltype(Color::R)* colorChannel;
	switch (count) {
	case 0:
		colorChannel = &c.R;
		break;
	case 1:
		colorChannel = &c.G;
		break;
	case 2:
		colorChannel = &c.B;
		break;
	default:
		colorChannel = nullptr;
		assert(false);
	}
	*colorChannel = static_cast<decltype(Color::R)>(input);
}


std::string ResourceManager::roomToString(const int x, const int y) {
	std::stringstream ss;
	ss << std::setfill('0') << std::setw(Constants::RMRoomLen) << x;
	ss << '_';
	ss << std::setfill('0') << std::setw(Constants::RMRoomLen) << y;
	return ss.str();
}


std::string ResourceManager::getPath(const ResourceType t, const std::string& name) {
	std::string path = GameData::instance().dataPath;
	switch (t) {
	case ResourceType::CREATURE:
		appendDir(path, "creatures");
		path += name;
		path += ".json";
		break;
	case ResourceType::FONT:
		appendDir(path, "fonts");
		path += name;
		break;
	case ResourceType::IMAGE:
		appendDir(path, "images");
		path += name;
		path += ".bmp";
		break;
	case ResourceType::IMAGE_KEY:
		appendDir(path, "images");
		path += name;
		path += ".txt";
		break;
	case ResourceType::ROOM:
		appendDir(path, "rooms");
		path += name;
		path += ".json";
		break;
	case ResourceType::SAVE:
		path = GameData::instance().savePath;
		path += name;	// name should already include extension
		break;
	case ResourceType::SPRITE:
		appendDir(path, "sprites");
		path += name;
		path += ".json";
		break;
	}
	return path;
}
