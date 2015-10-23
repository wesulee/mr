#include "vfx_manager.h"
#include "canvas.h"
#include "entity.h"


VFXManager::~VFXManager() {
	for (auto& v : list)
		delete v;
}


void VFXManager::update(const Constants::float_type dt) {
	for (auto it = list.begin(); it != list.end();) {
		if ((**it).update(dt)) {
			delete *it;
			it = list.erase(it);
		}
		else
			++it;
	}
}


void VFXManager::draw(Canvas& can) {
	for (auto vfx : list)
		vfx->draw(can);
}


void VFXManager::add(Entity* vfx) {
	list.push_back(vfx);
}
