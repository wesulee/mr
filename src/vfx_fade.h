#pragma once

#include "entity.h"
#include "utility.h"


class Image;


// This class is currently specifically for fading Spells
class VFXFade : public Entity {
public:
	VFXFade() = default;
	~VFXFade() {/* do nothing */}
	bool update(const Constants::float_type) override;
	void draw(Canvas&) override;
	void setImage(Image*, const int, const int);
	void setFade(const Constants::float_type, const Uint8, const Uint8);
	void setOffset(const int, const int);
	int getOffsetX(void) const;
	int getOffsetY(void) const;
private:
	AlphaFade af;
	IntPair imgDim;
	IntPair offset;
	Image* img = nullptr;
};
