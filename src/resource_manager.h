#pragma once

#include "font.h"
#include "sdl_helper.h"
#include "text_renderer.h"
#include <boost/functional/hash.hpp>
#include <cstddef>
#include <map>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>


class AnimatedSpriteSource;
enum class AnimationType;
class Color;
class CreatureData;
class FontResource;
class RoomData;
class SaveData;
class Sprite;
class SpriteSheet;
class UniformAnimatedSpriteSource;


template<class T>
struct ResourceCounter {
	// add constructors since brace-initialized list not working
	ResourceCounter() = default;
	ResourceCounter(T* r, const unsigned int c) : reference(r), counter(c) {}
	T* reference = nullptr;
	unsigned int counter = 0;
};


struct FontHash {
	std::size_t operator()(const Font& arg) const {
		std::size_t seed = 0;
		boost::hash_combine(seed, arg.name);
		boost::hash_combine(seed, arg.size);
		return seed;
	}
};


// manage loaded resources (sprites, fonts)
// NOTE: supports only one SpriteSheet
class ResourceManager {
	enum class ResourceType {CREATURE, FONT, IMAGE, IMAGE_KEY, ROOM, SAVE, SPRITE};
	enum class AnimationType {UNIFORM};
public:
	ResourceManager() = default;
	~ResourceManager();
	void init(void);
	SDL_Surface* getDefaultSurface(void);
	SpriteSheet* getSpriteSheet(const std::string&);
	TextRenderer* getDefaultTR(void);
	std::shared_ptr<RoomData> getRoomData(const int, const int);
	std::shared_ptr<CreatureData> getCreatureData(const std::string&);
	std::shared_ptr<SaveData> getSaveData(const std::string&);
	SDL_Surface* generateSurface(const RoomData&) const;
	FontResource loadFont(const Font&, const bool=true);
	void unloadFont(FontResource&);
	Sprite getSprite(const std::string&);
	UniformAnimatedSpriteSource* getUSprSrc(const std::string&);
	AnimatedSpriteSource* loadAnimation(const std::map<std::string, std::string>&);
	void saveSaveData(const std::string&, const SaveData&);
	void freeAnimation(const std::string&);
	void freeSpriteSheet(const std::string&);
private:
	std::pair<SDL_Surface*, SDL_Texture*> loadImage(const std::string&);
	SDL_Texture* autoLoadImage(const std::string&);
	SpriteSheet* loadSpriteSheet(const std::string&);
	AnimatedSpriteSource* loadAnimationUni(const std::map<std::string, std::string>&);
	void incTextureRef(const std::string&);
	void decTextureRef(const std::string&);
	TTF_Font* openFont(const Font&);
	static std::pair<bool, Color> readColorKey(const std::string&);
	static void readColorKeyProc(Color&, const unsigned int, const int);
	static std::string fontToString(const Font&);
	static std::string fontToString(const FontResource&);
	static std::string roomToString(const int, const int);
	static std::string getPath(const ResourceType, const std::string&);

	TextRenderer defaultTR;
	std::unordered_map<std::string, SDL_Rect> sprites;
	std::unordered_map<Font, ResourceCounter<TTF_Font>, FontHash> fonts;
	std::unordered_map<std::string, AnimatedSpriteSource*> animations;
	std::unordered_map<std::string, ResourceCounter<SDL_Texture>> textures;
	std::unordered_map<std::string, ResourceCounter<SpriteSheet>> sheets;
	std::unordered_map<std::string, AnimationType> animationLookup;
	std::unordered_set<TTF_Font*> fontsPrivate;
	SDL_Surface* defaultSurf = nullptr;
	SpriteSheet* defaultSS = nullptr;
};
