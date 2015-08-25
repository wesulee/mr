#pragma once

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
	virtual void update(void) = 0;
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
	void setTicks(const unsigned int);
	void add(const int, const int);
	// UniformAnimatedSprite methods
	void update(unsigned int&, std::size_t&) const;
	SDL_Texture* getTexture(void);
	SDL_Rect* getTextureBounds(const std::size_t);
	int getDrawWidth(void) const;
	int getDrawHeight(void) const;
private:
	std::vector<std::pair<int, int>> frames;
	SDL_Rect bounds;
	SDL_Texture* tex = nullptr;
	unsigned int ticksMax = 0;
};


// Every SDL_Rect is same size and all frame duration are same number of ticks.
class UniformAnimatedSprite : public AnimatedSprite {
public:
	void update(void) override;
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
	unsigned int ticks = 0;
};
