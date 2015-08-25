#include "font.h"
#include <cassert>


const std::string Font::DEFAULT = "EncodeSansNormal-400-Regular.ttf";
const std::string Font::DEFAULT_MONO = "DejaVuSansMono.ttf";


Font::Font(const std::string& n, const int s) : name(n), size(s) {
	assert(size > 0);
}


TextImage::TextImage(SDL_Texture* t, const int w, const int h) : tex(t), width(w), height(h) {
}


TextImage::~TextImage() {
	if (tex != nullptr) {
		SDL_DestroyTexture(tex);
		tex = nullptr;
	}
}


int TextImage::getWidth() const {
	return width;
}


int TextImage::getHeight() const {
	return height;
}


bool TextImage::empty() const {
	return tex == nullptr;
}
