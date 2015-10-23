#pragma once

#include "constants.h"
#include "drawable.h"
#include "sdl_header.h"
#include <string>
#include <utility>
#include <vector>


class Canvas;


class AnimatedSprite : public Drawable {
public:
	AnimatedSprite() {}
	virtual ~AnimatedSprite() {}
	virtual void update(const Constants::float_type) = 0;
	virtual void reset(void) = 0;
};


class AnimatedSpriteSource {
public:
	AnimatedSpriteSource() {}
	virtual ~AnimatedSpriteSource() {}
	void setImageName(const std::string& n) {imgName = n;}
	const std::string& getImageName() const {return imgName;}
private:
	std::string imgName;
};


// Animation with fixed frame dimensions
class UniformAnimatedSpriteSource : public AnimatedSpriteSource {
public:
	UniformAnimatedSpriteSource() = default;
	~UniformAnimatedSpriteSource();
	void setTexture(SDL_Texture*);	// does not take ownership of texture
	void setSize(const int, const int);
	void setDuration(const Constants::float_type);
	void add(const int, const int);
	// UniformAnimatedSprite methods
	void update(const Constants::float_type, Constants::float_type&, std::size_t&) const;
	SDL_Texture* getTexture(void);
	SDL_Rect* getTextureBounds(const std::size_t);
	int getDrawWidth(void) const;
	int getDrawHeight(void) const;
private:
	std::vector<std::pair<int, int>> frames;
	SDL_Rect bounds;
	SDL_Texture* tex = nullptr;
	Constants::float_type frameDur;	// duration of each frame
};


// Every SDL_Rect is same size and all frame duration are same number of ticks.
class UniformAnimatedSprite : public AnimatedSprite {
public:
	void update(const Constants::float_type) override;
	void reset(void) override;
	void setSource(UniformAnimatedSpriteSource*);
	// Drawable
	SDL_Texture* getTexture(void) override;
	SDL_Rect* getTextureBounds(void) override;
	int getDrawWidth(void) const override;
	int getDrawHeight(void) const override;
private:
	UniformAnimatedSpriteSource* src = nullptr;
	std::size_t index = 0;
	Constants::float_type frameTimeRem = 0;	// time remaining for current frame
};
