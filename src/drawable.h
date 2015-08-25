#pragma once


class SDL_Texture;
class SDL_Rect;


/*
Drawable objects can either be treated as objects with relative or absolute positions.
If the object has an absolute position, call Canvas(Drawable&).
If the object does not have an absolute position, call Canvas(Drawable&, int, int).
If the object has no absolute position and Canvas(Drawable&) is called, it should be drawn at (0, 0).
*/
class Drawable {
public:
	Drawable() = default;
	virtual ~Drawable() = default;
	virtual SDL_Texture* getTexture(void) = 0;
	virtual SDL_Rect* getTextureBounds(void) = 0;
	virtual int getDrawWidth(void) const = 0;
	virtual int getDrawHeight(void) const = 0;
	virtual int getDrawPosX() const {return 0;}
	virtual int getDrawPosY() const {return 0;}
};
