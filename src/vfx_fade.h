#pragma once

#include "entity.h"
#include "utility.h"
#include <utility>


class Image;


// This class is currently specifically for fading Spells
class VFXFade : public Entity {
public:
	VFXFade() = default;
	~VFXFade() {/* do nothing */}
	bool update(void) override;
	void draw(Canvas&) override;
	void setImage(Image*, const int, const int);
	void setFade(const Uint8, const Uint8, const unsigned int);
	void setOffset(const int, const int);
	int getOffsetX(void) const;
	int getOffsetY(void) const;
private:
	Image* img = nullptr;
	std::pair<int, int> imgDim;
	std::pair<int, int> offset;
	AlphaFade af;
};
