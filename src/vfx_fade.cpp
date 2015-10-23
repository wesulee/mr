#include "vfx_fade.h"
#include "canvas.h"
#include "image.h"
#include <cassert>


bool VFXFade::update(const Constants::float_type dt) {
	return af.update(dt);
}


// because a Spell's Image is shared, need to set attributes before drawing
void VFXFade::draw(Canvas& can) {
	assert(img != nullptr);
	img->setWidth(imgDim.first);
	img->setHeight(imgDim.second);
	img->setAlpha(af.getAlpha());
	can.draw(*img, offset.first, offset.second);
	img->setAlpha(SDL_ALPHA_OPAQUE);	// set back to default alpha
}


void VFXFade::setImage(Image* image, const int width, const int height) {
	img = image;
	imgDim.first = width;
	imgDim.second = height;
}


void VFXFade::setFade(const Constants::float_type dur, const Uint8 start, const Uint8 end) {
	af.set(dur, start, end);
}


void VFXFade::setOffset(const int x, const int y) {
	offset.first = x;
	offset.second = y;
}


int VFXFade::getOffsetX() const {
	return offset.first;
}


int VFXFade::getOffsetY() const {
	return offset.second;
}
