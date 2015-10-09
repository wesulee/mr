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
#include "logger.h"
#include "sprite.h"
#include "sprite_sheet.h"
#include "utility.h"
#include <boost/filesystem.hpp>
#include <cstdint>	// uintptr_t
#include <fstream>
#include <iomanip>
#include <sstream>
#include <vector>
#define BOOST_FILESYSTEM_NO_DEPRECATED


namespace ResManHelper {

template<class T, class Iter>
static void delSpriteSheet(T& map, Iter it) {
	delete it->second.res;
	map.erase(it);
}


template<typename T>
static std::string getHex(const T n) {
	std::stringstream ss;
	ss << std::hex << n << std::dec;
	return ss.str();
}


template<typename T>
static std::string ptrToStr(const T ptr) {
	if (ptr == nullptr)
		return "nullptr";
	else
		return ("0x" + getHex(reinterpret_cast<uintptr_t>(ptr)));
}


static std::string rectToStr(const SDL_Rect& r) {
	std::stringstream ss;
	ss << '(' << r.x << ", " << r.y << ", " << r.w << ", " << r.h << ')';
	return ss.str();
}

} // namespace ResManHelper


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
	for (auto it = fonts.begin(); it != fonts.end(); ++it)
		TTF_CloseFont(it->second.res);
	for (auto f : fontsPrivate)
		TTF_CloseFont(f);
	for (auto it = animations.begin(); it != animations.end(); ++it)
		delete it->second;
	for (auto it = sheets.begin(); it != sheets.end(); ++it)
		delete it->second.res;
	for (auto it = images.begin(); it != images.end(); ++it) {
		SDL::freeNull(it->second.res.surf);
		SDL::freeNull(it->second.res.tex);
	}
}


void ResourceManager::init() {
	// populate animation type mapping
	animationLookup.emplace("uni", AnimationType::UNIFORM);
}


UniformAnimatedSpriteSource* ResourceManager::getUSprSrc(const std::string& name) {
	auto it = animations.find(name);
	if (it == animations.end()) {
		Logger::instance().exit(RuntimeError{
			"invalid animation reference",
			"ResourceManager::getUSprSrc invalid reference to " + name
		});
	}
	return dynamic_cast<UniformAnimatedSpriteSource*>(it->second);
}


// Animations always load image as texture
AnimatedSpriteSource* ResourceManager::loadAnimation(const rapidjson::Value& data) {
	assert(data.HasMember("name"));
	assert(data.HasMember("img"));
	assert(data.HasMember("type"));
	auto it = animationLookup.find(data["type"].GetString());
	if (it == animationLookup.end()) {
		Logger::instance().exit(RuntimeError{
			"invalid animation type",
			"ResourceManager::loadAnimation unknown type: " + q(std::string{data["type"].GetString()})
		});
	}
	const AnimationType t = it->second;
	std::string name = data["name"].GetString();
	AnimatedSpriteSource* src;
	switch (t) {
	case AnimationType::UNIFORM:
		src = loadAnimationUni(data);
		break;
	default:
#if defined(DEBUG_RM_LOAD_ANIMATION) && DEBUG_RM_LOAD_ANIMATION
		DEBUG_BEGIN << DEBUG_RM_PREPEND << "loadAnimation type UNKNOWN " << q(name) << std::endl;
#endif
		assert(false);
		src = nullptr;
	}
	if (src == nullptr) {
		Logger::instance().exit(RuntimeError{
			"unable to load animation",
			"ResourceManager::loadAnimation name: " + name
		});
	}
	animations[name] = src;
	return src;
}


void ResourceManager::freeAnimation(const std::string& name) {
	auto it = animations.find(name);
	if (it == animations.end()) {
		Logger::instance().exit(RuntimeError{
			"invalid animation reference",
			"ResourceManager::freeAnimation cannot find: " + name
		});
	}
#if defined(DEBUG_RM_UNLOAD_ANIMATION) && DEBUG_RM_UNLOAD_ANIMATION
	DEBUG_BEGIN << DEBUG_RM_PREPEND << "unloadAnimation " << q(name) << std::endl;
#endif
	// decrement image reference
	auto itImg = images.find(it->second->getImageName());
	if (decImageCounter(itImg->second, false, true)) {
		images.erase(itImg);
	}
	delete it->second;
	animations.erase(it);
}


TextRenderer* ResourceManager::getDefaultTR() {
	return &defaultTR;
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
			++(it->second.count);
			fr.font = it->second.res;
#if defined(DEBUG_RM_LOAD_FONT) && DEBUG_RM_LOAD_FONT
			DEBUG_BEGIN << DEBUG_RM_PREPEND << "loadFont INC ref to " << it->second.count
			            << " (" << toString(fr) << ')' << std::endl;
#endif // DEBUG_RM_LOAD_FONT
		}
		else {	// hasn't been loaded yet, load now
			ResourceCounter<TTF_Font*> insert;
			insert.count = 1;
			insert.res = openFont(font);
			// insert into fonts
			fonts[font] = insert;
			fr.font = insert.res;
#if defined(DEBUG_RM_LOAD_FONT) && DEBUG_RM_LOAD_FONT
			DEBUG_BEGIN << DEBUG_RM_PREPEND << "loadFont NEW ref (" << toString(fr) << ')' << std::endl;
#endif
		}
	}
	else {	// if not shared, immediately load
		TTF_Font* f = openFont(font);
		fr.font = f;
		auto insert = fontsPrivate.insert(f);
		assert(insert.second);
		(void)insert;	// remove warning
#if defined(DEBUG_RM_LOAD_FONT) && DEBUG_RM_LOAD_FONT
		DEBUG_BEGIN << DEBUG_RM_PREPEND << "loadFont NEW (" << toString(fr) << ')' << std::endl;
#endif
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
#if defined(DEBUG_RM_UNLOAD_FONT) && DEBUG_RM_UNLOAD_FONT
			DEBUG_BEGIN << DEBUG_RM_PREPEND << "unloadFont invalid (" << toString(fr) << ')' << std::endl;
#endif
			Logger::instance().exit(RuntimeError{
				"FontResource error",
				"ResourceManager::unloadFont invalid resource: " + toString(fr)
			});
		}
		--(it->second.count);
		if (it->second.count == 0) {	// unload
#if defined(DEBUG_RM_UNLOAD_FONT) && DEBUG_RM_UNLOAD_FONT
			DEBUG_BEGIN << DEBUG_RM_PREPEND << "unloadFont FREE (" << toString(fr) << ')' << std::endl;
#endif
			TTF_CloseFont(it->second.res);
			fonts.erase(it);
		}
		else {
#if defined(DEBUG_RM_UNLOAD_FONT) && DEBUG_RM_UNLOAD_FONT
			DEBUG_BEGIN << DEBUG_RM_PREPEND << "unloadFont DEC ref to " << it->second.count
			            << " (" << toString(fr) << ')' << std::endl;
#endif // DEBUG_RM_UNLOAD_FONT
		}
	}
	else {
		auto it = fontsPrivate.find(fr.font);
		if (it == fontsPrivate.end()) {
#if defined(DEBUG_RM_UNLOAD_FONT) && DEBUG_RM_UNLOAD_FONT
			DEBUG_BEGIN << DEBUG_RM_PREPEND << "unloadFont invalid (" << toString(fr) << ')' << std::endl;
#endif
			Logger::instance().exit(RuntimeError{
				"FontResource error",
				"ResourceManager::unloadFont invalid resource: " + toString(fr)
			});
		}
		else {
#if defined(DEBUG_RM_UNLOAD_FONT) && DEBUG_RM_UNLOAD_FONT
			DEBUG_BEGIN << DEBUG_RM_PREPEND << "unloadFont FREE (" << toString(fr) << ')' << std::endl;
#endif
			TTF_CloseFont(*it);
			fontsPrivate.erase(it);
		}
	}
}


// surf=true when surface is needed, tex=true when texture is needed.
// If it is only used to generate a surface, then only surf should be true.
SpriteSheet* ResourceManager::getSpriteSheet(const std::string& name, const bool surf, const bool tex) {
	auto it = sheets.find(name);
	if (it != sheets.end()) {
		ImageResource* const ir = getImage(it->second.res->imgName, surf, tex);
		incSpriteSheetCounter(it->second, surf, tex);	// update sheet counter
		// surface or texture may have been loaded, so update spritesheet
		it->second.res->surf = ir->surf;
		it->second.res->tex = ir->tex;
		return it->second.res;
	}
	else {
		// not found, load
		return loadSpriteSheet(name, surf, tex);
	}
}


void ResourceManager::freeSpriteSheet(const std::string& name, const bool surf, const bool tex) {
	using namespace ResManHelper;
	auto it = sheets.find(name);
	if (it == sheets.end()) {
		Logger::instance().exit(RuntimeError{
			"invalid spritesheet reference",
			"ResourceManager::freeSpriteSheet cannot find " + q(name)
		});
	}
	decSpriteSheetCounter(it->second, surf, tex);
	if (it->second.countSurf == 0) {
		it->second.res->surf = nullptr;
		if (it->second.countTex == 0) {
			delSpriteSheet(sheets, it);
		}
	}
	if (it->second.countTex == 0) {
		it->second.res->tex = nullptr;
		if (it->second.countSurf == 0) {
			delSpriteSheet(sheets, it);
		}
	}
}


std::shared_ptr<rapidjson::Document> ResourceManager::getRoomData(const int x, const int y) {
	assert((x >= 0) && (y >= 0));
	assert((x < Constants::MapCountX) && (y < Constants::MapCountY));
	std::string filePath = getPath(ResourceType::ROOM, roomToString(x, y));
	std::shared_ptr<rapidjson::Document> data = JSONReader::read(filePath);
	if (!data)
		Logger::instance().exit(RuntimeError{"unable to load room", roomToString(x, y)});
#if defined(DEBUG_JSON_VALIDATE) && DEBUG_JSON_VALIDATE
	JSONReader::validateRoom(*data, filePath);
#endif
	return data;
}


std::shared_ptr<rapidjson::Document> ResourceManager::getCreatureData(const std::string& name) {
	std::string filePath = getPath(ResourceType::CREATURE, name);
	std::shared_ptr<rapidjson::Document> data = JSONReader::read(filePath);
	if (!data)
		Logger::instance().exit(RuntimeError{"unable to load creature " + q(name)});
	return data;
}


//! TODO remove
Sprite ResourceManager::getSprite(const std::string& name) {
	return sheets.at("default").res->get(name);
}


// if filePath is absolute path to item in data directory, return path relative to data dir
// else return empty
std::string ResourceManager::getRelDataPath(const std::string& filePath) {
	std::string ret;
	std::size_t i = filePath.find(GameData::instance().dataPath);
	if (i == 0)
		ret = filePath.substr(GameData::instance().dataPath.size());
	return ret;
}


#ifndef NDEBUG

// Print all information about loaded resources
void ResourceManager::printResources(std::ostream& os) const {
	using namespace ResManHelper;
	os << "===== BEGIN ResourceManager::printResources() =====" << std::endl;
	// print fonts
	std::size_t i = 0;
	os << "Member \"fonts\" (size " << fonts.size() << ')' << std::endl;
	for (auto it = fonts.cbegin(); it != fonts.cend(); ++it, ++i) {
		os << '\t' << i << " name: " << q(it->first.name) << " sz: " << it->first.size
		   << " TTF_Font*: " << ptrToStr(it->second.res) << " count: " << it->second.count
		   << std::endl;
	}
	os << std::endl;
	// print images
	i = 0;
	os << "Member \"images\" (size " << images.size() << ')' << std::endl;
	for (auto it = images.cbegin(); it != images.cend(); ++it) {
		os << '\t' << i << " name: " << q(it->first) << " countSurf: " << it->second.countSurf
		   << " countTex: " << it->second.countTex << " SDL_Surface*: " << ptrToStr(it->second.res.surf)
		   << " SDL_Texture*: " << ptrToStr(it->second.res.tex) << std::endl;
	}
	os << std::endl;
	// print animations
	i = 0;
	os << "Member \"animations\" (size " << animations.size() << ')' << std::endl;
	for (auto it = animations.cbegin(); it != animations.cend(); ++it, ++i) {
		os << '\t' << i << " name: " << q(it->first) << " AnimatedSpriteSource*: " << ptrToStr(it->second)
		   << " imgName: " << q(it->second->getImageName()) << std::endl;
	}
	os << std::endl;
	// print sheets
	os << "Member \"sheets\" (size " << sheets.size() << ')' << std::endl;
	for (auto it = sheets.cbegin(); it != sheets.cend(); ++it, ++i) {
		os << '\t' << i << " name: " << q(it->first) << " countSurf: " << it->second.countSurf << " countTex: "
		   << it->second.countTex << " SpriteSheet*: " << ptrToStr(it->second.res)
		   << " imgName: " << q(it->second.res->imgName) << " surf: " << ptrToStr(it->second.res->surf)
		   << " tex: " << ptrToStr(it->second.res->tex) << std::endl;
		os << "\t\tSPRITES ";
		if (it->second.res->sprites.empty())
			os << "empty!" << std::endl;
		for (auto it2 = it->second.res->sprites.cbegin(); it2 != it->second.res->sprites.cend(); ++it2) {
			os << q(it2->first) << ": " << rectToStr(it2->second) << ", ";
		}
		os << std::endl;
	}
	os << "===== END ResourceManager::printResources() =====" << std::endl;
}

#endif // NDEBUG


ResourceManager::ImageResource* ResourceManager::getImage(const std::string& name, const bool surf, const bool tex) {
	auto it = images.find(name);
	if (it != images.end()) {
		// Make sure that Image has the needed data
		if (surf && (it->second.res.surf == nullptr)) {
			// need to load surface, either from texture or from disk
			assert(false);	//! TODO not implemented
		}
		if (tex && (it->second.res.tex == nullptr)) {
			assert(it->second.res.surf != nullptr);
			it->second.res.tex = SDL::newTexture(it->second.res.surf);
		}
		incImgCounter(it->second, surf, tex);
		return &it->second.res;
	}
	else {
		return loadImage(name, surf, tex);
	}
}


ResourceManager::ImageResource* ResourceManager::loadImage(const std::string& name, const bool surf, const bool tex) {
	assert(images.find(name) == images.end());
	assert(!(!surf && !tex));	// at least one should be true
	std::string path = getPath(ResourceType::IMAGE, name);
	SDL_Surface* surface = SDL::loadBMP(path);
	std::pair<bool, Color> colorKey = readColorKey(name);
	if (colorKey.first && !SDL::setColorKey(surface, colorKey.second)) {
		SDL::logError("ResourceManager::loadImage SDL::setColorKey");
	}
#if defined(DEBUG_RM_IMG_REF) && DEBUG_RM_IMG_REF
	DEBUG_BEGIN << DEBUG_RM_PREPEND << DEBUG_RM_IMG_PREPEND << "NEW " << q(name) << std::endl;
#endif
	// keep track of references
	ImgCounter<ImageResource> ic;
	ic.res.name = name;
	if (surf) {
		ic.res.surf = surface;
		ic.countSurf = 1;
	}
	if (tex) {
		if (!surf) {
			ic.res.tex = SDL::toTexture(surface);
			ic.res.surf = nullptr;
		}
		else {
			ic.res.tex = SDL::newTexture(surface);
		}
		ic.countTex = 1;
	}
	auto p = images.insert(std::make_pair(name, ic));
	assert(p.second);	// check insert successful
	return &p.first->second.res;
}


SpriteSheet* ResourceManager::loadSpriteSheet(const std::string& name, const bool surf, const bool tex) {
	namespace rj = rapidjson;
	assert(sheets.find(name) == sheets.end());	// the sheet must not be loaded already
	std::string filePath = getPath(ResourceType::SPRITE, name);
	std::shared_ptr<rapidjson::Document> data = JSONReader::read(filePath);
	if (!data)
		Logger::instance().exit(RuntimeError{"unable to load spritesheet " + q(name)});
#if defined(DEBUG_JSON_VALIDATE) && DEBUG_JSON_VALIDATE
	JSONReader::validateSpriteSheet(*data, filePath);
#endif
	SpriteSheet* ss = new SpriteSheet;
	ss->imgName = (*data)["img"].GetString();
	{	// process sprites
		SDL_Rect tmpRect;
		std::string tmpStr;
		const rj::Value& sprites = (*data)["sprites"];
		for (rj::Value::ConstValueIterator it = sprites.Begin(); it != sprites.End(); ++it) {
			rj::Value::ConstValueIterator it2 = it->Begin();
			tmpStr = it2->GetString();
			++it2;
			JSONHelper::readRect(tmpRect, it2);
			ss->sprites.emplace(tmpStr, tmpRect);
		}
	}
	ImageResource* const ir = getImage(ss->imgName, surf, tex);
	ss->surf = ir->surf;
	ss->tex = ir->tex;
	// insert into sheets
	ImgCounter<SpriteSheet*> icSS;
	icSS.res = ss;
	icSS.countSurf = toCounterType(surf);
	icSS.countTex = toCounterType(tex);
	sheets[name] = icSS;
	return ss;
}


AnimatedSpriteSource* ResourceManager::loadAnimationUni(const rapidjson::Value& data) {
#if defined(DEBUG_RM_LOAD_ANIMATION) && DEBUG_RM_LOAD_ANIMATION
	DEBUG_BEGIN << DEBUG_RM_PREPEND << "loadAnimation type UNIFORM " << q(data["name"].GetString()) << std::endl;
#endif
	UniformAnimatedSpriteSource* src = new UniformAnimatedSpriteSource;
	src->setImageName(data["img"].GetString());
	int x, y, dx, dy, frames;
	ImageResource* const ir = getImage(src->getImageName(), false, true);	// get texture
	assert(ir->tex != nullptr);
	src->setTexture(ir->tex);
	src->setTicks(data["dur"].GetUint());
	src->setSize(data["w"].GetInt(), data["h"].GetInt());
	x = data["x"].GetInt();
	y = data["y"].GetInt();
	dx = data["dx"].GetInt();
	dy = data["dy"].GetInt();
	frames = data["frames"].GetInt();
	for (int i = 0; i < frames; ++i, x += dx, y += dy)
		src->add(x, y);
	return src;
}


void ResourceManager::incImageCounter(ImgCounter<ImageResource>& ic, const bool surf, const bool tex) {
	using namespace ResManHelper;
	incImgCounter(ic, surf, tex);
#if defined(DEBUG_RM_IMG_REF) && DEBUG_RM_IMG_REF
	DEBUG_BEGIN << DEBUG_RM_PREPEND << DEBUG_RM_IMG_PREPEND;
	if (surf)
		DEBUG_OS << "INC SURF " << ic.countSurf << ' ';
	if (tex)
		DEBUG_OS << "INC TEX " << ic.countTex << ' ';
	DEBUG_OS << "for " << q(ic.res.name) << std::endl;
#endif // DEBUG_RM_IMG_REF
}


// When return value true, erase this from map.
bool ResourceManager::decImageCounter(ImgCounter<ImageResource>& ic, const bool surf, const bool tex) {
	using namespace ResManHelper;
	decImgCounter(ic, surf, tex);
#if defined(DEBUG_RM_IMG_REF) && DEBUG_RM_IMG_REF
	DEBUG_BEGIN << DEBUG_RM_PREPEND << DEBUG_RM_IMG_PREPEND;
	if (surf)
		DEBUG_OS << "DEC SURF " << ic.countSurf << ' ';
	if (tex)
		DEBUG_OS << "DEC TEX " << ic.countTex << ' ';
	DEBUG_OS << "for " << q(ic.res.name) << std::endl;
#endif // DEBUG_RM_IMG_REF
	if (ic.countSurf == 0) {
		SDL::free(ic.res.surf);
		ic.res.surf = nullptr;
		if (ic.countTex == 0)
			return true;
	}
	if (ic.countTex == 0) {
		SDL::free(ic.res.tex);
		ic.res.tex = nullptr;
		if (ic.countSurf == 0)
			return true;
	}
	return false;
}


void ResourceManager::incSpriteSheetCounter(ImgCounter<SpriteSheet*>& ic, const bool surf, const bool tex) {
	using namespace ResManHelper;
	incImgCounter(ic, surf, tex);
#if defined(DEBUG_RM_SS_REF) && DEBUG_RM_SS_REF
	DEBUG_BEGIN << DEBUG_RM_PREPEND << DEBUG_RM_SS_PREPEND;
	if (surf)
		DEBUG_OS << "INC SURF " << ic.countSurf << ' ';
	if (tex)
		DEBUG_OS << "INC TEX " << ic.countTex << ' ';
	if (!surf && !tex)
		DEBUG_OS << "INC NO CHANGE ";
	DEBUG_OS << "for " << q(ic.res->imgName) << std::endl;
#endif // DEBUG_RM_SS_REF
}


// Note: this only decreases image references and spritesheet references
void ResourceManager::decSpriteSheetCounter(ImgCounter<SpriteSheet*>& ic, const bool surf, const bool tex) {
	using namespace ResManHelper;
	auto itImg = images.find(ic.res->imgName);
	decImgCounter(ic, surf, tex);
#if defined(DEBUG_RM_SS_REF) && DEBUG_RM_SS_REF
	DEBUG_BEGIN << DEBUG_RM_PREPEND << DEBUG_RM_SS_PREPEND;
	if (surf)
		DEBUG_OS << "DEC SURF " << ic.countSurf << ' ';
	if (tex)
		DEBUG_OS << "DEC TEX " << ic.countTex << ' ';
	DEBUG_OS << "for " << q(ic.res->imgName) << std::endl;
#endif
	if (decImageCounter(itImg->second, surf, tex))
		images.erase(itImg);
}


TTF_Font* ResourceManager::openFont(const Font& font) {
	std::string filePath = getPath(ResourceType::FONT, font.name);
	return SDL::openFont(filePath, font.size);
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
	case ResourceType::SPRITE:
		appendDir(path, "sprites");
		path += name;
		path += ".json";
		break;
	}
	return path;
}
