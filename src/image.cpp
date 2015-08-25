#include "image.h"


Image::Image(SDL_Surface* surf) {
	assert(surf != nullptr);
	width = surf->w;
	height = surf->h;
	tex = SDL_CreateTextureFromSurface(SDL::renderer, surf);
	if (tex == nullptr) {
		SDL::logError("Image::Image SDL_CreateTextureFromSurface");
	}
}


Image::Image(SDL_Texture* texture) {
	assert(texture != nullptr);
	SDL_QueryTexture(texture, nullptr, nullptr, &width, &height);
	tex = texture;
}


Image::~Image() {
	SDL::freeNull(tex);
}
