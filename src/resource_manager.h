#pragma once

#include "font.h"
#include "json_reader.h"
#include "sdl_helper.h"
#include "text_renderer.h"
#include <boost/functional/hash.hpp>
#include <cassert>
#include <cstddef>
#include <iostream>		// printResources()
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <utility>


class AnimatedSpriteSource;
enum class AnimationType;
class Color;
class FontResource;
class Sprite;
class SpriteSheet;
class UniformAnimatedSpriteSource;


// Manage resources (images, fonts, animations, spritesheets)
// When images are loaded, must specify if surface and/or texture is needed.
class ResourceManager {
	typedef unsigned int counter_type;
	template<class T>
	struct ResourceCounter {
		// add constructors since brace-initialized list not working
		ResourceCounter() = default;
		ResourceCounter(T r, const counter_type c) : res(r), count(c) {}

		T res;
		counter_type count = 0;
	};

	struct FontHash {
		std::size_t operator()(const Font& arg) const {
			std::size_t seed = 0;
			boost::hash_combine(seed, arg.name);
			boost::hash_combine(seed, arg.size);
			return seed;
		}
	};

	struct ImageResource {
		std::string name;
		SDL_Surface* surf;
		SDL_Texture* tex;
	};

	template<class T>
	struct ImgCounter {
		T res;
		counter_type countSurf = 0;
		counter_type countTex = 0;
	};

	static constexpr counter_type toCounterType(const bool v) {
		return (v ? 1 : 0);
	}

	template<class T>
	void incImgCounter(T& ic, const bool surf, const bool tex) {
		ic.countSurf += toCounterType(surf);
		ic.countTex += toCounterType(tex);
	}

	template<class T>
	void decImgCounter(T& ic, const bool surf, const bool tex) {
		// make sure that it is ok to decrement
		assert(!(surf && (ic.countSurf == 0)));
		assert(!(tex && (ic.countTex == 0)));
		ic.countSurf -= toCounterType(surf);
		ic.countTex -= toCounterType(tex);
	}

	enum class ResourceType {CREATURE, FONT, IMAGE, IMAGE_KEY, ROOM, SPRITE};
	enum class AnimationType {UNIFORM};
public:
	ResourceManager() = default;
	~ResourceManager();
	void init(void);
	// animation
	UniformAnimatedSpriteSource* getUSprSrc(const std::string&);
	AnimatedSpriteSource* loadAnimation(const rapidjson::Value&);
	void freeAnimation(const std::string&);
	// font
	TextRenderer* getDefaultTR(void);
	FontResource loadFont(const Font&, const bool=true);
	void unloadFont(FontResource&);
	// spritesheet
	SpriteSheet* getSpriteSheet(const std::string&, const bool, const bool);
	void freeSpriteSheet(const std::string&, const bool, const bool);
	// other
	std::shared_ptr<rapidjson::Document> getRoomData(const int, const int);
	std::shared_ptr<rapidjson::Document> getCreatureData(const std::string&);
	Sprite getSprite(const std::string&);
	std::string getRelDataPath(const std::string&);
	void printResources(std::ostream&) const;
private:
	ImageResource* getImage(const std::string&, const bool, const bool);
	ImageResource* loadImage(const std::string&, const bool, const bool);
	SpriteSheet* loadSpriteSheet(const std::string&, const bool, const bool);
	AnimatedSpriteSource* loadAnimationUni(const rapidjson::Value&);
	void incImageCounter(ImgCounter<ImageResource>&, const bool, const bool);
	bool decImageCounter(ImgCounter<ImageResource>&, const bool, const bool);
	void incSpriteSheetCounter(ImgCounter<SpriteSheet*>&, const bool, const bool);
	void decSpriteSheetCounter(ImgCounter<SpriteSheet*>&, const bool, const bool);
	TTF_Font* openFont(const Font&);
	static std::pair<bool, Color> readColorKey(const std::string&);
	static void readColorKeyProc(Color&, const unsigned int, const int);
	static std::string fontToString(const Font&);
	static std::string fontToString(const FontResource&);
	static std::string roomToString(const int, const int);
	static std::string getPath(const ResourceType, const std::string&);

	TextRenderer defaultTR;
	std::unordered_map<Font, ResourceCounter<TTF_Font*>, FontHash> fonts;
	std::unordered_map<std::string, AnimatedSpriteSource*> animations;
	std::unordered_map<std::string, ImgCounter<ImageResource>> images;
	std::unordered_map<std::string, ImgCounter<SpriteSheet*>> sheets;
	std::unordered_map<std::string, AnimationType> animationLookup;
	std::unordered_set<TTF_Font*> fontsPrivate;
};
